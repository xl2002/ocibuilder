#include "image/types/blobcache.h"

/**
 * @brief 创建新的BlobCache对象
 * 
 * @param ref 镜像引用对象
 * @param directory 缓存目录路径
 * @param compress 压缩配置
 * @return std::shared_ptr<BlobCache> 新创建的BlobCache对象指针
 */
std::shared_ptr<BlobCache> NewBlobCache(std::shared_ptr<ImageReference_interface> ref, std::string directory, std::shared_ptr<LayerCompression> compress){
    return nullptr;
}

/**
 * @brief 获取传输协议内部的引用字符串
 * 
 * @return std::string 传输协议内部的引用字符串
 */
std::string BlobCache::StringWithinTransport(){
    return "";
}
/**
 * @brief 获取策略配置标识
 * 
 * @return std::string 策略配置标识字符串
 */
std::string BlobCache::PolicyConfigurationIdentity(){
    return "";
}
/**
 * @brief 获取策略配置命名空间列表
 * 
 * @return std::vector<std::string> 命名空间字符串列表
 */
std::vector<std::string> BlobCache::PolicyConfigurationNamespaces(){
    return std::vector<std::string>();
}
/**
 * @brief 获取Docker引用对象
 * 
 * @return std::shared_ptr<Named_interface> Docker引用对象指针
 */
std::shared_ptr<Named_interface> BlobCache::DockerReference(){
    return nullptr;
}
/**
 * @brief 获取传输协议对象
 * 
 * @return std::shared_ptr<ImageTransport_interface> 传输协议对象指针
 */
std::shared_ptr<ImageTransport_interface> BlobCache::Transport(){
    return nullptr;
}
/**
 * @brief 创建新的镜像对象
 * 
 * @param sys 系统上下文
 * @return std::shared_ptr<Image_interface> 新创建的镜像对象指针
 */
std::shared_ptr<Image_interface> BlobCache::NewImage(std::shared_ptr<SystemContext>sys){
    return nullptr;
}
/**
 * @brief 创建新的镜像源对象
 * 
 * @param sys 系统上下文
 * @param check 是否检查镜像有效性
 * @return std::shared_ptr<ImageSource_interface> 新创建的镜像源对象指针
 */
std::shared_ptr<ImageSource_interface> BlobCache::NewImageSource(std::shared_ptr<SystemContext>sys,bool check){
    return nullptr;
}
/**
 * @brief 创建新的镜像目标对象
 * 
 * @param sys 系统上下文
 * @return std::shared_ptr<ImageDestination_interface> 新创建的镜像目标对象指针
 */
std::shared_ptr<ImageDestination_interface> BlobCache::NewImageDestination(std::shared_ptr<SystemContext>sys){
    return nullptr;
}
/**
 * @brief 获取缓存目录路径
 * 
 * @return std::string 缓存目录路径字符串
 */
std::string BlobCache::Directory(){
    return "";
}
