#include "image/image_types/docker/docker_transport.h"
#include "utils/common/error.h"
#include "utils/common/go/string.h"
std::string dockerTransport::Name() {
    return "docker";
}
std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<myerror>> newReference(std::shared_ptr<Named_interface>ref,bool unknownDigest){
    auto docker=std::make_shared<dockerReference>();
    docker->ref=ref;
    docker->isUnknownDigest=unknownDigest;
    return std::make_tuple(docker, nullptr);
    
    // return std::make_tuple(nullptr, nullptr);
}
std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<myerror>> NewReference(std::shared_ptr<Named_interface>ref) {
    return newReference(ref, false);
}
std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<myerror>> ParseReference(std::string refString) {
    
    if(!hasPrefix(refString,"//")){
        return std::make_tuple(nullptr,std::make_shared<myerror>("Invalid reference \""+refString+"\""));
    }
    bool unknownDigest = false;
    try{
        auto ref=ParseNormalizedNamed(TrimPrefix(refString,"//"));
        if(!ref){
            return std::make_tuple(nullptr,std::make_shared<myerror>("Invalid reference \""+refString+"\""));
        }
        return NewReference(ref);
        // return std::make_tuple(nullptr,nullptr);
    }catch(std::exception &e){
        return std::make_tuple(nullptr,std::make_shared<myerror>("Invalid reference \""+refString+"\""));
    }
}
std::shared_ptr<ImageReference_interface> dockerTransport::ParseReference(const std::string &reference) {
    std::shared_ptr<ImageReference_interface> ref;
    std::shared_ptr<myerror> err;
    std::tie(ref,err) =::ParseReference(reference);
    return ref;
}

void dockerTransport::ValidatePolicyConfigurationScope(const std::string &scope) {
    return;
}
std::shared_ptr<ImageTransport_interface> dockerReference::Transport() {
    return  docker_Transport;
}
std::string dockerReference::StringWithinTransport(){
    return  "";
}
std::string dockerReference::PolicyConfigurationIdentity(){
    return "";
}
std::vector<std::string> dockerReference::PolicyConfigurationNamespaces(){
    return std::vector<std::string>();
}
std::shared_ptr<Named_interface> dockerReference::DockerReference(){
    return ref;
}
std::shared_ptr<Image_interface> dockerReference::NewImage(std::shared_ptr<SystemContext>sys){
    return nullptr;
}
std::shared_ptr<ImageSource_interface> dockerReference::NewImageSource(std::shared_ptr<SystemContext>sys) {
    return nullptr;
}
std::shared_ptr<ImageDestination_interface> dockerReference::NewImageDestination(std::shared_ptr<SystemContext>sys) {
    return nullptr;
}
