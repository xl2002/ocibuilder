#include "image/util/util.h"
#include "filesys/systems.h"
#include <sys/stat.h>
#include <vector>
#include "utils/common/go/file.h"
// #include <minwindef.h>
#include "filesys/systems.h"
// #include <fileapi.h>
#include <pthread.h>
#include <boost/filesystem.hpp>
/**
 *        保存在authfile中
 * @param file  需要被复制的文件
 * @param authfile 保存临时文件的路径
 * @return 如果给定的文件是指向Windows系统的文件描述符，
 *         则返回true，否则返回false
 */
bool MirrorToTempFileIfPathIsDescriptor(std::string file ,std::string &authfile){
    if(file.substr(0,8)!="/dev/fd/"){
        authfile=file;
        return false;
    }
    try
    {
        std::ifstream infile(file,std::ios::binary);
        if(!infile.is_open()){
            throw myerror("Unable to open file: " + file);
        }
        infile.close();
    }
    catch(const myerror & e)
    {
        throw;
    }
    return true;
}


/**
 * @brief DiscoverContainerfile 该函数用来查找 Containerfile 文件
 *        如果给定的文件是一个目录，则查找该目录下的 Containerfile 文件
 *        如果给定的文件是一个普通文件，则直接返回该文件
 *        如果给定的文件既不是目录也不是普通文件
 *        则抛出异常
 * @param path 该参数是要查找的文件路径
 * @return std::string 如果查找成功，返回 Containerfile 的路径
 * @throws myerror 如果给定的文件既不是目录也不是普通文件
 */
std::string DiscoverContainerfile(std::string path){
    struct stat target;
    std::string foundCtrFile;
    if(stat(path.c_str(),&target)!=0){
        throw myerror("discovering Containerfile: " + std::string(strerror(errno)));
    }
    if((target.st_mode& S_IFDIR)!=0){
        std::string ctrfile = joinPath(path, "Containerfile");
        if (!fileExists(ctrfile)) {
            ctrfile = joinPath(path, "Dockerfile");
            if (!fileExists(ctrfile)) {
                throw myerror("cannot find Containerfile or Dockerfile in context directory");
            }
        }

        if (isRegularFile(ctrfile)) {
            foundCtrFile = ctrfile;
        } else {
            throw myerror("assumed Containerfile " + ctrfile + " is not a file");
        }
    } else if ((target.st_mode & S_IFREG) != 0) {
        foundCtrFile = path;
    } else {
        throw myerror("path is neither a regular file nor a directory");
    }
    return foundCtrFile;
}


/**
 * @brief 解析符号链接
 * @param path 文件路径
 * @return 解析后的绝对路径
 */
std::string resolveSymlinks(const std::string& path) {
    // 在 Windows 和 Linux 平台下，Boost Filesystem 提供统一的 API 处理符号链接
    try {
        boost::filesystem::path boostPath(path);
        
        // 解析符号链接，返回绝对路径
        boost::filesystem::path absolutePath = boost::filesystem::canonical(boostPath);
        
        return absolutePath.string();
    } catch (const boost::filesystem::filesystem_error& e) {
        throw std::runtime_error("Error resolving symlinks: " + std::string(e.what()));
    }
}

/**
 * @brief 验证并解析镜像标签名称
 * 
 * @param imagespec 镜像名称字符串
 * @return std::shared_ptr<ImageReference_interface> 返回解析后的镜像引用对象
 * @throws myerror 如果镜像名称无法解析
 * @note 会尝试两种方式解析：
 *       1. 直接解析输入的名称
 *       2. 添加默认传输前缀后再次尝试解析
 */
std::shared_ptr<ImageReference_interface> VerifyTagName(std::string imagespec){
    // try
    // {
    //     return ParseImageName(imagespec);
    // }
    // catch(const myerror& e)
    // {   try{
    //         return ParseImageName(DefaultTransport+imagespec);
    //     }
    //     catch(const myerror& e)
    //     {
    //         throw;
    //     }
    // }
    std::shared_ptr<ImageReference_interface> ref;
    ref=ParseImageName(imagespec);
    if(ref==nullptr){
        ref=ParseImageName(DefaultTransport+imagespec);
        if(ref==nullptr){
            throw myerror("failed to parse image name: "+imagespec);
        }
    }
    return ref;
}

/**
 * @brief 标准化平台信息
 * 
 * @param platform 原始平台信息
 * @return Platform 返回标准化后的平台信息
 * @note 当前实现直接返回输入参数，需要根据实际需求完善
 */
Platform NormalizePlatform(Platform platform){

    return platform;
}

/**
 * @brief 检查集合中是否包含指定元素
 * 
 * @param m 要检查的字符串集合
 * @param k 要查找的元素
 * @return bool 如果集合中包含元素则返回true，否则返回false
 */
bool SetHas(std::set<std::string> m,std::string k){
    return m.find(k)!=m.end();
}
