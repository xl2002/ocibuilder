#include "imagebuildah/build.h"
#include "cli/common.h"
#include "pkg/parse/parse.h"
#include "logrus/logger.h"
#include "io/io.h"
#include "src/strings.h"
string BuildDockerfiles(Store* store, define_BuildOptions options,vector<string> paths,Canonical* ret_ref){
    auto ctx = getContext();

    if (options.CommonBuildOpts == nullptr)
    {
        //将options.CommonBuildOpts初始化重新为CommonBuildOptions 
    }
    std::shared_ptr<int> ptr = Volumes(options.CommonBuildOpts->Volumes);
    if (ptr != nullptr)
    {
        return "";
    }
    if (paths.size() == 0)
    {
        return "";
    }
    if (options.PlatformsList.size() > 1 && options.IIDFile != "")
    {
        return "";
    }
    
    auto logger = Logger::New();
    if (options.Err != nullptr)
    {
        logger->SetOutput(options.Err);//基础类结构-改define_BuildOptions
    }
    else
    {
        //logger->SetOutput(os.Stderr);
        //os库，未执行
    }
    auto level = logger->GetLevel();
    logger->SetLevel(level);
    
    
    std::vector<std::shared_ptr<ReadCloser>> dockerfiles;
    //defer
    try {
        // 使用 Defer 类在离开作用域时自动关闭所有文件
        Defer defer([&dockerfiles]() {
            for (const auto& d : dockerfiles) {
                d->Close();
            }
        });
        // 资源会在离开作用域时自动关闭
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    // 将 Output 和 AdditionalTags 合并到一个向量中
    std::vector<std::string> tags = {options.Output};
    tags.insert(tags.end(), options.AdditionalTags.begin(), options.AdditionalTags.end());

    // 遍历并处理 tags 向量
    for (const auto& tag : tags) {
        if (tag.empty()) {
            continue;
        }
        // if (util.VerifyTAgName(tag)!=nullptr)
        // {
        //     return "";
        // }
        //
    }
    
    //读dockerfile文件
    for (const auto& dfile : paths)
    {
        std::shared_ptr<ReadCloser> data;
        //判断dfile是否为远程路径
        if (HasPrefix(dfile,"http://") || HasPrefix(dfile,"https://"))
        {
            //以调试级别记录一条日志消息
            //logger->Debugf("reading remote Dockerfile %q", dfile);

        }else{
            try
            {
                auto dinfo = Stat(dfile);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            
            
        }
        
    }
    

    return "";
}
