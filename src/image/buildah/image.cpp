#include "image/buildah/buildah.h"
#include "image/buildah/image.h"


std::shared_ptr<containerImageRef> Builder::makeContainerImageRef(std::shared_ptr<CommitOptions> options){

    return nullptr;
}
std::shared_ptr<ImageTransport_interface> containerImageRef::Transport(){
    return nullptr;
}
std::string containerImageRef::StringWithinTransport(){
    return "";
}
std::string containerImageRef::PolicyConfigurationIdentity(){
    return "";
}
std::vector<std::string> containerImageRef::PolicyConfigurationNamespaces(){
    return {};
}
std::shared_ptr<Named_interface> containerImageRef::DockerReference(){
    return nullptr;
}