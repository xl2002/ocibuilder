#include "storage/storage/storage_reference.h"
#include "utils/common/error.h"
#include "storage/storage/storage.h"
std::shared_ptr<ImageTransport_interface> storageReference::Transport(){
    auto st=std::make_shared<storageTransport>();
    st->store=this->transport->store;
    st->defaultUIDMap=this->transport->defaultUIDMap;
    st->defaultGIDMap=this->transport->defaultGIDMap;
    return st;
}

std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<storage::Image>>ResolveReference(std::shared_ptr<ImageReference_interface>ref){
    auto sref=std::dynamic_pointer_cast<storageReference>(ref);
    if(sref!=nullptr) {
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
std::shared_ptr<Image_interface> newImage(std::shared_ptr<SystemContext>sys,std::shared_ptr<storageReference>ref){
    return nullptr;
}
std::shared_ptr<Image_interface> storageReference::NewImage(std::shared_ptr<SystemContext>sys){
    return newImage(sys,std::make_shared<storageReference>(*this));
}
std::shared_ptr<ImageSource_interface> newImageSource(std::shared_ptr<SystemContext>sys,std::shared_ptr<storageReference>ref){
    return nullptr;
}
std::shared_ptr<ImageSource_interface> storageReference::NewImageSource(std::shared_ptr<SystemContext>sys){
    return newImageSource(sys,std::make_shared<storageReference>(*this));
}
/**
 * @brief 在tmp目录下创建缓存目录，并且构建storageImageDestination
 * 
 * @return std::shared_ptr<storageImageDestination> 
 */
std::shared_ptr<storageImageDestination> newImageDestination(std::shared_ptr<SystemContext>sys,std::shared_ptr<storageReference>ref){
    
}
/**
 * @brief 建立镜像够简单的缓存目录
 * 
 * @return std::shared_ptr<ImageDestination_interface> 
 */
std::shared_ptr<ImageDestination_interface> storageReference::NewImageDestination(std::shared_ptr<SystemContext>sys){
    return newImageDestination(sys,std::make_shared<storageReference>(*this));
}
std::string storageReference::StringWithinTransport(){
    return "";
}
std::string storageReference::PolicyConfigurationIdentity(){
    return "";
}
std::vector<std::string> storageReference::PolicyConfigurationNamespaces(){
    return {};
}
std::shared_ptr<Named_interface> storageReference::DockerReference(){
    return nullptr;
}
