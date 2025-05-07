#include "storage/storage/storage_reference.h"
#include "utils/common/error.h"
#include "storage/storage/storage.h"

#include "utils/common/go/file.h"
/**
 * @brief 获取存储传输接口
 * @return std::shared_ptr<ImageTransport_interface> 返回存储传输接口指针
 */
std::shared_ptr<ImageTransport_interface> storageReference::Transport(){
    auto st=std::make_shared<storageTransport>();
    st->store=this->transport->store;
    st->defaultUIDMap=this->transport->defaultUIDMap;
    st->defaultGIDMap=this->transport->defaultGIDMap;
    return st;
}

/**
 * @brief 解析存储引用
 * @param ref 输入的图像引用接口指针
 * @return std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<storage::Image>> 返回解析后的引用和图像指针
 */
std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<storage::Image>>ResolveReference(std::shared_ptr<ImageReference_interface>ref){
    auto sref=std::dynamic_pointer_cast<storageReference>(ref);
    if(sref==nullptr) {
        return std::make_tuple(nullptr,nullptr);
    }
    auto clone=sref;
    try{
        auto img=clone->resolveImage(nullptr);
        return std::make_tuple(clone,img);
    }catch(const myerror &e){
        throw;
    }
    // return std::make_tuple(ref,std::make_shared<storage::Image>());
}
/**
 * @brief 解析存储中的图像
 * @param sys 系统上下文指针
 * @return std::shared_ptr<storage::Image> 返回解析后的图像指针
 */
std::shared_ptr<storage::Image> storageReference::resolveImage(std::shared_ptr<SystemContext>sys){
    std::shared_ptr<storage::Image> loadedImage=nullptr;
    if(this->id==""&&this->named!=nullptr){
        try{
            auto image=this->transport->store->Image(this->named->String());
            if(image!=nullptr){
                loadedImage=image;
                this->id=image->ID;
            }
        }catch(const myerror &e){   
            throw;
        }
    }
    auto digest=loadedImage->BigDataDigests.find("manifest");
    if(digest!=loadedImage->BigDataDigests.end()){
        loadedImage->digest=std::make_shared<Digest>(digest->second);
    }
    if(this->named!=nullptr){
        auto digested=std::dynamic_pointer_cast<Digested_interface>(this->named);
        if(digested!=nullptr){
            loadedImage->digest=std::make_shared<Digest>(digested->Digest());
        }
    }
    return loadedImage;
}
/**
 * @brief 创建新图像
 * @param sys 系统上下文指针
 * @param ref 存储引用指针
 * @return std::shared_ptr<Image_interface> 返回新创建的图像接口指针
 */
std::shared_ptr<Image_interface> newImage(std::shared_ptr<SystemContext>sys,std::shared_ptr<storageReference>ref){
    return nullptr;
}
/**
 * @brief 创建新图像
 * @param sys 系统上下文指针
 * @return std::shared_ptr<Image_interface> 返回新创建的图像接口指针
 */
std::shared_ptr<Image_interface> storageReference::NewImage(std::shared_ptr<SystemContext>sys){
    return newImage(sys,std::make_shared<storageReference>(*this));
}
/**
 * @brief 创建新图像源
 * @param sys 系统上下文指针
 * @param ref 存储引用指针
 * @return std::shared_ptr<ImageSource_interface> 返回新创建的图像源接口指针
 */
std::shared_ptr<ImageSource_interface> newImageSource(std::shared_ptr<SystemContext>sys,std::shared_ptr<storageReference>ref){
    return nullptr;
}
/**
 * @brief 创建新图像源
 * @param sys 系统上下文指针
 * @param check 是否进行检查的标志
 * @return std::shared_ptr<ImageSource_interface> 返回新创建的图像源接口指针
 */
std::shared_ptr<ImageSource_interface> storageReference::NewImageSource(std::shared_ptr<SystemContext>sys,bool check){
    return newImageSource(sys,std::make_shared<storageReference>(*this));
}

/**
 * @brief 在tmp目录下创建缓存目录，并且构建storageImageDestination
 * 
 * @return std::shared_ptr<storageImageDestination> 
 */
std::shared_ptr<storageImageDestination> newImageDestination(std::shared_ptr<SystemContext>sys,std::shared_ptr<storageReference>ref){
    auto tmpdir="";
    auto dest=std::make_shared<storageImageDestination>();
    dest->imageRef=ref;
    dest->directory=tmpdir;
    return dest;
}
/**
 * @brief 建立镜像够简单的缓存目录
 * 
 * @return std::shared_ptr<ImageDestination_interface> 
 */
std::shared_ptr<ImageDestination_interface> storageReference::NewImageDestination(std::shared_ptr<SystemContext>sys){
    return newImageDestination(sys,std::make_shared<storageReference>(*this));
}
/**
 * @brief 获取传输中的字符串表示
 * @return std::string 返回传输中的字符串表示
 */
std::string storageReference::StringWithinTransport(){
    return "";
}
/**
 * @brief 获取策略配置标识
 * @return std::string 返回策略配置标识字符串
 */
std::string storageReference::PolicyConfigurationIdentity(){
    return "";
}
/**
 * @brief 获取策略配置命名空间
 * @return std::vector<std::string> 返回策略配置命名空间列表
 */
std::vector<std::string> storageReference::PolicyConfigurationNamespaces(){
    return {};
}
/**
 * @brief 获取Docker引用
 * @return std::shared_ptr<Named_interface> 返回Docker引用接口指针
 */
std::shared_ptr<Named_interface> storageReference::DockerReference(){
    return this->named;
}
