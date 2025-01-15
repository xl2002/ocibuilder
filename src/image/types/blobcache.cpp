#include "image/types/blobcache.h"

std::shared_ptr<BlobCache> NewBlobCache(std::shared_ptr<ImageReference_interface> ref, std::string directory, std::shared_ptr<LayerCompression> compress){
    return nullptr;
}

std::string BlobCache::StringWithinTransport(){
    return "";
}
std::string BlobCache::PolicyConfigurationIdentity(){
    return "";
}
std::vector<std::string> BlobCache::PolicyConfigurationNamespaces(){
    return std::vector<std::string>();
}
std::shared_ptr<Named_interface> BlobCache::DockerReference(){
    return nullptr;
}
std::shared_ptr<ImageTransport_interface> BlobCache::Transport(){
    return nullptr;
}
std::shared_ptr<Image_interface> BlobCache::NewImage(std::shared_ptr<SystemContext>sys){
    return nullptr;
}
std::shared_ptr<ImageSource_interface> BlobCache::NewImageSource(std::shared_ptr<SystemContext>sys,bool check){
    return nullptr;
}
std::shared_ptr<ImageDestination_interface> BlobCache::NewImageDestination(std::shared_ptr<SystemContext>sys){
    return nullptr;
}
std::string BlobCache::Directory(){
    return "";
}