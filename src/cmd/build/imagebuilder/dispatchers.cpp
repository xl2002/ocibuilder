#include "cmd/build/imagebuilder/dispatchers.h"
#include "cmd/build/imagebuilder/internals.h"
#include "utils/cli/cobra/lex.h"
#include <boost/algorithm/string.hpp>
#include "utils/logger/ProcessSafeLogger.h"

// ENV foo bar
//
// Sets the environment variable foo to bar, also makes interpolation
// in the dockerfile available from the next statement on via ${foo}.
void env(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs)
{
    if(args.size()==0){
        throw errAtLeastOneArgument("ENV");
    }
    if(args.size()%2!=0){
        throw errTooManyArguments("ENV");
    }
    for(int i=0;i<args.size();i++){
        auto newVar=args[i] + "=" + args[i + 1];
        // 将新环境变量添加到 b->RunConfig.Env 和 b->Env 中
        std::vector<std::string> newEnvVar{newVar};
        b->RunConfig->Env = mergeEnv(b->RunConfig->Env, newEnvVar);
        b->Env = mergeEnv(b->Env, newEnvVar);

        i++;  // 跳过下一个元素，因为它是当前 name 的值
    }
    return;
}

// FROM imagename
//
// This sets the image the dockerfile will build on top of.
void from(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs)
{
    // 确保传入的参数符合要求
    if (!(args.size() == 1 || (args.size() == 3 && !args[0].empty() && boost::iequals(args[1], "as") && !args[2].empty()))) {
        throw std::runtime_error("FROM requires either one argument, or three: FROM <source> [as <name>]");
    }

    std::string name = args[0];

    // 支持在 FROM 之前使用 ARG
    std::vector<std::string> argStrs;
    for (const auto& pair : b->HeadingArgs) {
        argStrs.push_back(pair.first + "=" + pair.second);
    }
    std::vector<std::string> defaultArgs = envMapAsSlice(builtinBuildArgs);
    std::map<std::string, std::string> filteredUserArgs;

    // 过滤允许的用户参数
    for (const auto& pair : b->UserArgs) {
        for (const auto& allowedArg : b->GlobalAllowedArgs) {
            if (allowedArg == pair.first) {
                filteredUserArgs[pair.first] = pair.second;
            }
        }
    }

    // 合并用户环境变量
    std::vector<std::string> userArgs = mergeEnv(envMapAsSlice(filteredUserArgs), b->Env);
    userArgs = mergeEnv(defaultArgs, userArgs);
    std::vector<std::string> nameArgs = mergeEnv(argStrs, userArgs);

    // 处理镜像名称
    try {
        name = ProcessWord(name, nameArgs);
    } catch (const std::exception& e) {
        throw std::runtime_error(e.what());
    }
    // std::string dest = makeAbsolute(name, b->RunConfig->WorkingDir);

    std::string chown;
    std::string chmod;
    std::string from;
    // std::vector<std::string> userArgs = mergeEnv(envMapAsSlice(b->Args), b->Env);



    // Windows 不支持没有基础镜像的容器
//     if (name == "scratch") {
// #ifdef _WIN32
//         throw std::runtime_error("Windows does not support FROM scratch");
// #endif
//     }

    // 处理标志参数
    for (const auto& a : flagArgs) {
        std::string arg = ProcessWord(a, userArgs);
        if (boost::algorithm::starts_with(arg, "--platform=")) {
            std::string platformString = arg.substr(11);  // 去掉 "--platform="
            if (platformString.empty()) {
                throw std::runtime_error("no value specified for --platform=");
            }
            b->Platform = platformString;
        } else {
            throw std::runtime_error("FROM only supports the --platform flag");
        }
    }
    // 设置镜像名称
    b->RunConfig->Image = name;

    // 将拷贝操作添加到待处理的列表中
    // if(b->PendingCopies.size()==0&&name!="scratch"){
    //     boost::filesystem::path src(name);
    //     auto absrc=boost::filesystem::absolute(src);
    //     Copy c;
    //     c.From = "from";
    //     c.Src.push_back(absrc.string());
    //     c.Dest = "";
    //     c.Download = false;
    //     c.Chown = chown;
    //     c.Chmod = chmod;
    //     // c.Files = files;
    //     b->PendingCopies.push_back(c);
    // }
    // TODO: 处理 onbuild
}
// LABEL some json data describing the image
//
// Sets the Label variable foo to bar,
void label(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs)
{
    // 检查参数长度是否为 0
    if (args.empty()) {
        throw errAtLeastOneArgument("LABEL");
    }

    // 检查参数长度是否为偶数，确保每个 name 都有对应的 value
    if (args.size() % 2 != 0) {
        throw errTooManyArguments("LABEL");
    }

    // 如果 Labels 是空的，初始化为一个空的 map
    if (b->RunConfig->Labels.empty()) {
        b->RunConfig->Labels = std::map<std::string, std::string>{};
    }

    // 遍历 args，按 name 和 value 配对
    for (size_t j = 0; j < args.size(); j += 2) {
        // name  ==> args[j]
        // value ==> args[j+1]
        b->RunConfig->Labels[args[j]] = args[j + 1];
    }
}
// COPY foo /path
//
// Same as 'ADD' but without the tar and remote url handling.
void dispatchCopy(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs)
{
    if (args.size() < 2) {
        throw errAtLeastTwoArgument("COPY");
    }

    // 获取最后一个参数作为目标路径
    size_t last = args.size() - 1;
    std::string dest = makeAbsolute(args[last], b->RunConfig->WorkingDir);

    std::string chown;
    std::string chmod;
    std::string from;
    std::vector<std::string> userArgs = mergeEnv(envMapAsSlice(b->Args), b->Env);

    // 处理标志参数
    for (const auto& a : flagArgs) {
        std::string arg = ProcessWord(a, userArgs);
        if (boost::algorithm::starts_with(arg, "--chown=")) {
            chown = arg.substr(8);  // 移除 "--chown=" 前缀
            if (chown.empty()) {
                throw myerror("no value specified for --chown=");
            }
        } else if (boost::algorithm::starts_with(arg, "--chmod=")) {
            chmod = arg.substr(8);  // 移除 "--chmod=" 前缀
            checkChmodConversion(chmod);  // 如果转换失败，将会抛出异常
        } else if (boost::algorithm::starts_with(arg, "--from=")) {
            from = arg.substr(7);  // 移除 "--from=" 前缀
            if (from.empty()) {
                throw myerror("no value specified for --from=");
            }
        } else {
            throw myerror("COPY only supports the --chmod=<permissions> --chown=<uid:gid> and the --from=<image|stage> flags");
        }
    }

    // 处理 heredocs 文件
    auto files = processHereDocs(original, heredocs, userArgs);

    // 将拷贝操作添加到待处理的列表中
    Copy c;
    c.From = "copy";
    c.Src= std::vector<std::string>(args.begin(), args.begin() + last);
    c.Dest = dest;
    c.Download = false;
    c.Chown = chown;
    c.Chmod = chmod;
    c.Files = files;
    b->PendingCopies.push_back(c);
    // b->PendingCopies.push_back(Copy{from, std::vector<std::string>(args.begin(), args.begin() + last), dest, false, chown, chmod, files});
}
// EXPOSE 6667/tcp 7000/tcp
//
// Expose ports for links and port mappings. This all ends up in
// b.RunConfig.ExposedPorts for runconfig.
void expose(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs)
{
    if (args.empty()) {
        throw errAtLeastOneArgument("EXPOSE");
    }

    // 如果 ExposedPorts 为 null，初始化为空的 map
    if (b->RunConfig->ExposedPorts.empty()) {
        b->RunConfig->ExposedPorts = std::vector<Port>();
    }

    // 记录现有的端口
    std::set<std::string> existingPorts;
    for (auto& entry : b->RunConfig->ExposedPorts) {
        existingPorts.insert(entry.port());
    }

    // 处理每个端口
    for (auto& portStr : args) {
        Port dp(portStr);
        std::string portKey = dp.port()+ "/" + dp.proto();
        if (existingPorts.find(portKey) == existingPorts.end()) {
            b->RunConfig->ExposedPorts.push_back(Port(portKey));
        }
    }
}
// ENTRYPOINT /usr/sbin/nginx
//
// Set the entrypoint (which defaults to sh -c on linux, or cmd /S /C on Windows) to
// /usr/sbin/nginx. Will accept the CMD as the arguments to /usr/sbin/nginx.
//
// Handles command processing similar to CMD and RUN, only b.RunConfig.Entrypoint
// is initialized at NewBuilder time instead of through argument parsing.
void entrypoint(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs)
{
    std::vector<std::string> parsed = handleJSONArgs(args, attributes);

    if (attributes.at("json")) {
        // ENTRYPOINT ["echo", "hi"]
        b->RunConfig->Entrypoint = parsed;
    } else if (parsed.empty()) {
        // ENTRYPOINT []
        b->RunConfig->Entrypoint.clear();
    } else {
        // ENTRYPOINT echo hi
        if (/* runtime.GOOS != "windows" */ true) { // 这里使用条件判断模拟
            b->RunConfig->Entrypoint = {"/bin/sh", "-c", parsed[0]};
        } else {
            b->RunConfig->Entrypoint = {"cmd", "/S", "/C", parsed[0]};
        }
    }

    // 当设置 ENTRYPOINT 时，如果没有显式设置 CMD，则将 CMD 设为 null
    if (!b->CmdSet) {
        b->RunConfig->Cmd.clear();
    }
}
// VOLUME /foo
//
// Expose the volume /foo for use. Will also accept the JSON array form.
void Volume(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs)
{
    if(args.size()==0){
        throw errAtLeastOneArgument("VOLUME");
    }
    if(b->RunConfig->Volumes.empty()){
        b->RunConfig->Volumes = std::map<std::string, std::string>();
    }
    for(auto& v:args){
        v=TrimSpace(v);
        std::string src,dest;
        std::tie(src, dest,std::ignore)=Cut(v,':');
        if(v==""){
            throw myerror("Volume specified can not be an empty string");
        }
        b->RunConfig->Volumes[src]=dest;
        b->PendingVolumes->Add(src);
    }
}
void workdir(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs)
{
    if(args.size()!=1){
        throw errExactlyOneArgument("WORKDIR");
    }
    boost::filesystem::path workdir(args[0]);
    if(!workdir.is_absolute()&&workdir.string().substr(0, 1) != "/"){
        workdir=b->RunConfig->WorkingDir + workdir.string().substr(1);
    }
    // if(!boost::filesystem::is_directory(workdir)){
    //     boost::filesystem::create_directories(workdir);
    // }
    b->RunConfig->WorkingDir=workdir.string();
    return;
}
// 处理 HereDoc 的函数
std::vector<File> processHereDocs(const std::string& originalInstruction, const std::vector<Heredoc>& heredocs, const std::vector<std::string>& args) {
    std::vector<File> files;
    for (const auto& heredoc : heredocs) {
        std::string content = heredoc.Content;

        // 处理 Chomp 选项
        if (heredoc.Chomp) {
            // content = ChompHeredocContent(content);
        }

        // 处理 Expand 选项
        if (heredoc.Expand) {
            try {
                auto shlex=NewLex('\\');
                shlex->RawQuotes = true;
			    shlex->RawEscapes = true;
                content = shlex->ProcessWord(content, args);
            } catch (const myerror& e) {
                throw ;
            }
        }

        // 构造 File 对象
        File file = {content, heredoc.Name};
        files.push_back(file);
    }
    return files;
}
// 检查 chmod 的转换是否正确
void checkChmodConversion(const std::string& chmod) {
    try {
        std::size_t pos;
        unsigned long result = std::stoul(chmod, &pos, 8);  // 以8进制解析
        if (pos != chmod.size()) {
            throw std::invalid_argument("Error parsing chmod");
        }
    } catch (const std::invalid_argument&) {
        throw myerror("Error parsing chmod " + chmod);
    }
}

// 返回 "command 至少需要一个参数" 错误
myerror errAtLeastOneArgument(const std::string& command) {
    logger->log_error(command + " requires at least one argument");
    return myerror(command + " requires at least one argument");
}

// 返回 "command 至少需要两个参数" 错误
myerror errAtLeastTwoArgument(const std::string& command) {
    logger->log_error(command + " requires at least two arguments");
    return myerror(command + " requires at least two arguments");
}

// 返回 "command 只需要一个参数" 错误
myerror errExactlyOneArgument(const std::string& command) {
    logger->log_error(command + " requires exactly one argument");
    return myerror(command + " requires exactly one argument");
}

// 返回 "command 参数过多" 错误
myerror errTooManyArguments(const std::string& command) {
    logger->log_error("Bad input to " + command + ", too many arguments");
    return myerror("Bad input to " + command + ", too many arguments");
}

// 返回 "command 参数需要为 JSON 格式" 错误
myerror errNotJSON(const std::string& command) {
    logger->log_error(command + " requires the arguments to be in JSON form");
    return myerror(command + " requires the arguments to be in JSON form");
}