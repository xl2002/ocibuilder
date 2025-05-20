/**
 * @file config.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 config.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "cmd/config/config.h"
#include "utils/cli/cli/common.h"
#include "image/buildah/buildah.h"
#include "storage/storage/storage.h"
#include "utils/logger/ProcessSafeLogger.h"
// #include "command.h"

/**
 * @brief 初始化 config 命令的内容
 *
 * 这个函数用于初始化 config 命令的内容，它创建了一个 config 命令对象，并将其添加到根命令中。
 * 该命令对象具有以下属性：
 * - 命令名称为 "config"
 * - 简短介绍为 "Update image configuration settings"
 * - 详细介绍为 "Modifies the configuration values which will be saved to the image."
 * - 使用示例如下：
 *   - buildah config --author='Jane Austen' --workingdir='/etc/mycontainers' containerID
 *   - buildah config --entrypoint '[ \"/entrypoint.sh\", \"dev\" ]' containerID
 *   - buildah config --env foo=bar --env PATH=$PATH containerID
 *
 * 函数内部不执行 config 命令的 Run 操作，因此需要在其他地方定义 configCmd 函数。
 *
 * @return void
 */
void init_configcmd(){
    auto options=std::make_shared<configOptions>();
    string name{"config"};
    string Short{"Update image configuration settings"};
    string Long{"Modifies the configuration values which will be saved to the image."};
    string example{"ocibuilder config --author='Jane Austen' --workingdir='/etc/mycontainers' containerID\n  ocibuilder config --entrypoint '[ \"/entrypoint.sh\", \"dev\" ]' containerID\n  ocibuilder config --env foo=bar --env PATH=$PATH containerID"};
    
    auto configCommand=std::make_shared<Command>(name,Short,Long,example);
    string Template=UsageTemplate();
    configCommand->SetUsageTemplate(Template);
    auto flags=configCommand->Flags();
    flags->SetInterspersed(false);
    //初始化每个flag的内容
    flags->BoolVar(options->addHistory,"add-history",false,"add an entry for this operation to the image's history.  Use BUILDAH_HISTORY environment variable to override. (default false)");
    flags->StringArrayVar(options->annotation,"annotation",vector<string>(),"add `annotation` e.g. annotation=value, for the target image (default [])");
    flags->StringVar(options->arch,"arch","","set the architecture of the image (e.g., amd64, arm, ppc64le) (default unset)");
    flags->StringVar(options->author,"author","","set the author of the image (e.g., Jane Austen) (default unset)");
    flags->StringVar(options->cmd,"cmd","","set the default `command` to run for containers based on the image");
    flags->StringArrayVar(options->env,"env",vector<string>(), "set an environment variable for the target image (e.g., foo=bar) (default [])");
    flags->StringVar(options->entrypoint,"entrypoint","", "set the entrypoint for the target image (default [])");
    flags->StringArrayVar(options->label,"label",vector<string>(), "set a label for the target image (e.g., label=value) (default [])");
    flags->StringVar(options->os,"os","","set the OS of the image (e.g., linux, windows) (default unset)");
    
    configCommand->Run=[=](std::shared_ptr<Command> cmd, vector<string> args){
        configCmd(cmd,args,options);
    };
    rootcmd->AddCommand({configCommand});
    // return configCommand;
}
/**
 * @brief 加载镜像库中的镜像，构建新的Builder对象
 * 
 * @param store 
 * @param name 
 * @return std::shared_ptr<Builder> 
 */
std::shared_ptr<Builder> openBuilder(std::shared_ptr<Store> store,std::string name){
    //imagestore中存储了镜像的信息，包括config，manifest等
    auto imagestore=store->Image(name);

    //构建新的Builder对象，根据imagestore中的config和manifest，构建新的Builder对象
    auto builder = std::make_shared<Builder>();
    builder->OCIv1 = imagestore->image_config;
    builder->store =store;

    return builder;
}
/**
 * @brief 手动解析 [args1,arg2] 格式的字符串到字符串数组
 * 
 * 该函数用于解析类似JSON数组格式的字符串输入，但不需要严格的JSON格式。
 * 主要处理以下情况：
 * 1. 空输入 - 返回空数组
 * 2. 非数组格式输入 - 返回包含该字符串的单元素数组
 * 3. 数组格式输入 - 解析并返回字符串数组
 * 
 * @param input 要解析的输入字符串
 * @return std::vector<std::string> 解析后的字符串数组
 */
std::vector<std::string> parseManualArray(const std::string& input) {
    // 检查输入是否为空
    if (input.empty()) {
        return {};
    }

    // 如果输入没有括号，则直接将输入作为一个单一字符串
    if (input.front() != '[' || input.back() != ']') {
        return {input};  // 输入不是数组格式，直接返回一个包含该字符串的数组
    }

    // 如果输入是数组格式
    std::vector<std::string> result;
    std::string current;
    bool inElement = false;

    // 遍历数组中的字符，去掉开头和结尾的方括号
    for (size_t i = 1; i < input.size() - 1; ++i) { // 去掉两边的 [ 和 ]
        char c = input[i];

        if (std::isspace(c)) {
            continue; // 忽略空白字符
        }

        if (c == ',') {
            if (inElement) {
                result.push_back(current);
                current.clear();
                inElement = false;
            }
        } else {
            current += c;
            inElement = true;
        }
    }

    // 如果最后还有未处理的元素
    if (!current.empty()) {
        result.push_back(current);
    }

    return result;
}
/**
 * @brief 更新Builder对象中的Entrypoint配置
 * 
 * 该函数根据输入的entrypoint字符串更新Builder对象的Entrypoint配置。
 * 处理以下情况：
 * 1. 空字符串 - 清空Entrypoint
 * 2. JSON数组格式字符串 - 解析为字符串数组设置Entrypoint
 * 3. 普通字符串 - 直接设置为Entrypoint
 * 
 * @param builder 要更新的Builder对象
 * @param entrypoint 新的Entrypoint配置字符串
 */
void updateEntrypoint(std::shared_ptr<Builder> builder, const std::string& entrypoint) {
    if (entrypoint.empty()) {
        // 如果 entrypoint 为空，设置为空
        builder->SetEntrypoint({});
        return;
    }

    // 尝试将 entrypoint 解析为 JSON 数组
    try {
        // 手动解析 [args1,arg2] 格式的字符串
        std::vector<std::string> entrypointVec = parseManualArray(entrypoint);
        builder->SetEntrypoint(entrypointVec);
        std::cout << "Entrypoint set as array." << std::endl;
    } catch (const std::exception& e) {
        // 如果解析失败，则将 entrypoint 作为普通字符串传递给 shell
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        // std::vector<std::string> entrypointVec = {"/bin/sh", "-c", entrypoint};
        std::vector<std::string> entrypointVec = {entrypoint};
        builder->SetEntrypoint(entrypointVec);

        // std::cout << "Entrypoint set as shell command: " << entrypoint << std::endl;
    }
}
/**
 * @brief 更新Builder对象中的镜像配置
 * 
 * @param builder 
 * @param c 
 * @param iopts 
 */
void updateConfig(std::shared_ptr<Builder> builder, std::shared_ptr<Command> c, std::shared_ptr<configOptions> iopts) {
    try {
        // 根据 iopts 中的配置更新 builder 中的镜像配置

        // 更新 author
        if (c->flags->Changed("author")) {
            builder->SetMaintainer(iopts->author);
        }

        // 更新 architecture
        if (c->flags->Changed("arch")) {
            builder->SetArchitecture(iopts->arch);
        }

        // 更新 os
        if (c->flags->Changed("os")) {
            builder->SetOS(iopts->os);
        }

        // 更新 env
        if (c->flags->Changed("env")) {
            for (const auto& envSpec : iopts->env) {
                auto env = split(envSpec, '='); // 假设您有一个 split 函数可以分割字符串
                if (env.size() > 1) {
                    builder->SetEnv(env[0], env[1]);
                } else {
                    // 错误处理或默认行为，您可以选择抛出 myerror 异常
                    throw myerror("setting env " + env[0] + ": no value given");
                }
            }
        }

        // 更新 label
        if (c->flags->Changed("label")) {
            for (const auto& labelSpec : iopts->label) {
                auto label = split(labelSpec, '='); // 假设您有一个 split 函数
                if (label.size() > 1) {
                    builder->SetLabel(label[0], label[1]);
                } else {
                    // 错误处理或默认行为，您可以选择将空字符串设置为默认值
                    builder->SetLabel(label[0], "");
                }
            }
        }

        // 更新 entrypoint
        if (c->flags->Changed("entrypoint")) {
            updateEntrypoint(builder, iopts->entrypoint);
        }

    } catch (const myerror& e) {
        // 处理 myerror 类型的异常
        // 此处您可以根据需要进行日志记录或其他处理
        std::cerr << "Error: " << e.what() << std::endl;
        throw; // 重新抛出异常以供外层捕获
    }
}



/**
 * @brief config命令Run操作的
 * 
 */
void configCmd(std::shared_ptr<Command> cmd, std::vector<std::string> args, std::shared_ptr<configOptions> iopts) {
    try {
        logger->set_module("config");
        logger->log_info("Starting config command execution");
        
        // 1. 检查是否提供了有效的容器ID参数
        if (args.empty()) {
            logger->log_error("Container ID must be specified");
            throw myerror("container ID must be specified");
        }
        if (args.size() > 1) {
            logger->log_warning("Too many arguments specified, only first will be used");
            throw myerror("too many arguments specified");
        }
        std::string name = args[0];

        // 2. 获取存储对象（store）
        auto store = getStore(cmd);
        if (!store) {
            logger->log_error("Failed to get store");
            throw myerror("failed to get store");
        }

        // 3. 创建Builder对象
        auto builder = openBuilder(store, name);
        if (!builder) {
            logger->log_error("Failed to create builder for container: " + name);
            throw myerror("reading build container failed: " + name);
        }

        // 4. 更新配置
        updateConfig(builder, cmd, iopts);

        // 5. 保存镜像配置
        builder->Save(name);
        // delete iopts;
        logger->log_info("Config command completed successfully");
    } catch (const myerror& e) {
        // 输出错误信息并终止执行
        logger->log_error(std::string(e.what()));
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }
}
