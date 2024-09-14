#include "storage/storage_reference.h"

std::shared_ptr<ImageTransport_interface> storageReference::Transport(){
    auto st=std::make_shared<storageTransport>();
    st->store=this->transport->store;
    st->defaultUIDMap=this->transport->defaultUIDMap;
    st->defaultGIDMap=this->transport->defaultGIDMap;
    return st;
}

std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<storage::Image>>ResolveReference(std::shared_ptr<ImageReference_interface>ref){
    return std::make_tuple(ref,std::make_shared<storage::Image>());
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