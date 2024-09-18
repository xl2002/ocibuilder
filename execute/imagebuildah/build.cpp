#include "imagebuildah/build.h"
#include <mutex>
#include <future>
#include "cli/common.h"
#include "parse/parse.h"
#include "logrus/logger.h"
#include <fstream>
string BuildDockerfiles(shared_ptr<store> stores, shared_ptr<define_BuildOptions> options,vector<string> paths,shared_ptr<Canonical> ret_ref){
    //auto ctx = getContext();

    if (options->CommonBuildOpts == nullptr)
    {
        //将options.CommonBuildOpts初始化重新为CommonBuildOptions 
    }
    try
    {
        Volumes(options->CommonBuildOpts->Volumes);
    }
    catch(const myerror& e)
    {
        throw myerror("validating volumes: "+string(e.what()));
    }
    
    if (paths.empty())
    {
        throw myerror("building: no dockerfiles specified");
    }
    if (options->PlatformsList.size() > 1 && options->IIDFile != "")
    {
        throw myerror ("building multiple images, but iidfile %q can only be used to store one image ID "+ options->IIDFile);
    }
    
    // auto logger = Logger::New();
    // if (options->Err != nullptr)
    // {
    //     logger->SetOutput(options->Err);//基础类结构-改define_BuildOptions
    // }
    // else
    // {
    //     //logger->SetOutput(os.Stderr);
    //     //os库，未执行
    // }
    // auto level = logger->GetLevel();
    // logger->SetLevel(level);
    std::vector<std::unique_ptr<std::ifstream>> dockerfiles;
    auto deleter = [&]() {
        for (auto& f : dockerfiles) {
            f->close();
        }
    };
    auto tags=options->AdditionalTags;
    tags.emplace_back(options->Output);
    // for (const auto& tag : tags) {
    //     if (tag.empty()) {
    //         continue;
    //     }
    //     try {
    //         util::VerifyTagName(tag);
    //     } catch (const std::exception& e) {
    //         return "", nullptr, fmt::format("tag {} error: {}", tag, e.what());
    //     }


    // // 将 Output 和 AdditionalTags 合并到一个向量中
    // std::vector<std::string> tags = {options->Output};
    // tags.insert(tags.end(), options->AdditionalTags.begin(), options->AdditionalTags.end());

    // // 遍历并处理 tags 向量
    // for (const auto& tag : tags) {
    //     if (tag.empty()) {
    //         continue;
    //     }
    //     // if (util.VerifyTAgName(tag)!=nullptr)
    //     // {
    //     //     return "";
    //     // }
    //     //
    // }
    
    //读dockerfile文件
    for (const auto& dfile : paths)
    {
        std::unique_ptr<std::ifstream> data;
        //判断dfile是否为远程路径
        if (hasPrefix(dfile,"http://") || hasPrefix(dfile,"https://"))
        {
            //以调试级别记录一条日志消息
            //logger->Debugf("reading remote Dockerfile %q", dfile);

        }else{
            struct stat dInfo;
            if (stat(dfile.c_str(), &dInfo) != 0){
               //如果Docker文件不可用，请预先设置上下文目录(如果尚未设置的话)
                if (hasPrefix(dfile, options->ContextDirectory) )//未执行
                {
                    const std::string dfile = joinPath(options->ContextDirectory, dfile);
                }   
            }
            if (stat(dfile.c_str(), &dInfo) != 0){
                    return "";
            }
            
            // 使用 unique_ptr 管理文件指针
            std::unique_ptr<std::ifstream> contents(new std::ifstream(dfile));
            // 检查文件是否成功打开
            if (!contents->is_open()) {
                return "";
            }
            // 获取文件信息
            if (stat(dfile.c_str(), &dInfo) != 0) {
                contents->close();
                return "";
            }
            if (S_ISREG(dInfo.st_mode) && dInfo.st_size == 0) {
                // 检查文件是否是常规文件且大小为0
                contents->close();
                return "";
            }
            // 数据指针
            std::ifstream* data = contents.get();
        }
        if (hasSuffix(dfile,".in"))
        {   //未执行
            auto pData = preprocessContainerfileContext(/*logger, */dfile, data,  options->ContextDirectory,  options->CPPFlags);
            // if (pData == nullptr){
            //     return "";
            // }
            //data=NopCloser(pData);
        }
        
        dockerfiles.push_back(std::move(data));
    }
            
    //读取dockerfile内容,按ascii码存储...
    std::vector<std::vector<char>> files;
    for (const auto& dockerfile : dockerfiles)
    {
        std::stringstream buffer;
        buffer << dockerfile->rdbuf();  // 读取文件内容到字符串流
        std::string content = buffer.str();

        // 将字符串流内容转换为 char 向量
        std::vector<char> fileContent(content.begin(), content.end());
        files.push_back(std::move(fileContent));
    }
    
    if (options->JobSemaphore==nullptr)
    {
        if (options->Jobs != nullptr)
        {
            if (*options->Jobs < 0)
            {
                return "";
            }
            if (*options->Jobs > 0)
            {
                options->JobSemaphore = NewWeighted(int64_t(*options->Jobs));
            }
        }else
        {
            options->JobSemaphore = NewWeighted(1);
        }
    }
    
    auto manifestList = options->Manifest;
    options->Manifest = "";
    class instance{
        //Platform;json之类；
        std::string ID;
        std::unique_ptr<Canonical> ref;
    };
    std::vector<instance> instances;
    std::mutex instanceLock;
    Group builds;
    builds.Wait();
    if(options->SystemContext == nullptr){
        options->SystemContext = std::make_shared<SystemContext>();
    }
    if(options->PlatformsList.size() == 0){
        options->PlatformsList.emplace_back(options->SystemContext->OSChoice, options->SystemContext->ArchitectureChoice,"");
    }//OS, Arch, Variant string
    
    if(options->AllPlatforms){//未执行
        if(options->AdditionalBuildContexts.empty()){
            options->AdditionalBuildContexts[" "] = std::make_shared<AdditionalBuildContext>();
        }
        try{options->PlatformsList = platformsForBaseImages(paths,files,options->From,options->Args,options->AdditionalBuildContexts,options->SystemContext);}
        catch(myerror e){
            throw myerror(e.what());
        }
    }

    auto systemContext = options->SystemContext;
    for (const auto& platform : options->PlatformsList)
    {
        auto platformContext = *systemContext;
        
    }
    
    deleter();
    return "";
}


std::unique_ptr<ifstream> preprocessContainerfileContext(/*Logger logger, */string containerfile, const std::unique_ptr<std::ifstream>& r,  string ctxDir,  vector<string> cppflags){

    return nullptr;
}
// void buildDockerfilesOnce(shared_ptr<store> stores,std::shared_ptr<Logger> logger,string logPrefix,shared_ptr<define_BuildOptions> options,vector<string> containerFiles,vector<vector<uint8_t>> dockerfilecontents,string& ret_thisID,shared_ptr<Canonical> ret_ref){
    
// }


vector<platforms> platformsForBaseImages(
    const std::vector<std::string>& dockerfilepaths,
    const std::vector<std::vector<char>>& dockerfiles,
    const std::string& from,
    const map<string,string>& args,
    const map<string,shared_ptr<AdditionalBuildContext>>& additionalBuildContext,
    const std::shared_ptr<SystemContext>& systemContext
){

}