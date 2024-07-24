#include "imagebuildah/build.h"
#include <mutex>
#include <future>
#include "cli/common.h"
#include "parse/parse.h"
#include "logrus/logger.h"
#include <fstream>
string BuildDockerfiles(shared_ptr<store> stores, shared_ptr<define_BuildOptions> options,vector<string> paths,shared_ptr<Canonical> ret_ref){
    // auto ctx = getContext();

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
    
    // //读dockerfile文件
    // for (const auto& dfile : paths)
    // {
    //     std::shared_ptr<ReadCloser> data;
    //     //判断dfile是否为远程路径
    //     if (HasPrefix(dfile,"http://") || HasPrefix(dfile,"https://"))
    //     {
    //         //以调试级别记录一条日志消息
    //         //logger->Debugf("reading remote Dockerfile %q", dfile);

    //     }else{
    //         try
    //         {
    //             auto dinfo = Stat(dfile);
    //         }
    //         catch(const std::exception& e)
    //         {
    //             std::cerr << e.what() << '\n';
    //         }
            
            
    //     }
        
    // }
    deleter();
    return "";
}



// void buildDockerfilesOnce(shared_ptr<store> stores,std::shared_ptr<Logger> logger,string logPrefix,shared_ptr<define_BuildOptions> options,vector<string> containerFiles,vector<vector<uint8_t>> dockerfilecontents,string& ret_thisID,shared_ptr<Canonical> ret_ref){
    
// }
