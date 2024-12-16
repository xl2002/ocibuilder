#include "image/buildah/buildah.h"
#include "utils/common/go/string.h"
#include "utils/common/go/file.h"
#include "boost/filesystem.hpp"
/**
 * @brief 将destination的内容添加到mountPoint的diff文件夹中
 * 
 * @param destination 
 * @param extract 
 * @param options 
 * @param sources 
 */
void Builder::Add(std::string destination,bool extract,std::shared_ptr<AddAndCopyOptions> options,std::vector<std::string> sources){
//这个函数复杂，简单的按照文件复制处理
    auto mountPoint=this->MountPoint;
    auto contextDir=options->ContextDir;
    auto extractDirectory=boost::filesystem::path(mountPoint+"/diff") / boost::filesystem::path(destination);
    if(!boost::filesystem::exists(extractDirectory)){
        boost::filesystem::create_directories(extractDirectory);
    }
    for(auto source:sources){
        auto srcpath=boost::filesystem::path(source);
        if(!boost::filesystem::exists(srcpath)){
            std::cout<<"source is not exist"<<std::endl;
            continue;
        }
        if(boost::filesystem::is_directory(srcpath)){
            Copy_directory(srcpath,extractDirectory);
        }else{
            // const boost::filesystem::path& sourcePath = srcpath.path();
            boost::filesystem::path destinationPath = extractDirectory / srcpath.filename();
            boost::filesystem::copy_file(srcpath,destinationPath, boost::filesystem::copy_option::overwrite_if_exists);
        }
    }
}