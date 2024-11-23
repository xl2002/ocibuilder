#include "storage/storage_transport.h"
#include "go/string.h"
// const std::shared_ptr<StoreTransport_interface> Transport= std::make_shared<storageTransport>();

std::string storageTransport::Name(){
    return "containers-storage";
}
std::shared_ptr<storageReference>storageTransport::ParseStoreReference(std::shared_ptr<Store_interface> store,const std::string &reference){
    auto img=store->Image(reference);
    std::string id;
    if(img!=nullptr&&hasPrefix(img->ID,reference)){ 
        id=img->ID;
    }
    std::shared_ptr<Named_interface> named=nullptr;
    auto result=this->NewStoreReference(store,named,id);
    if(result!=nullptr){
        return result;
    }
    return nullptr;
}

// 将字符串转换为ImageReference
std::shared_ptr<ImageReference_interface> storageTransport::ParseReference(const std::string &reference){

    return nullptr;
}

// 验证策略配置范围
void storageTransport::ValidatePolicyConfigurationScope(const std::string &scope){
    
}
std::shared_ptr<storageReference>newReference(std::shared_ptr<storageTransport> transport,std::shared_ptr<Named_interface> named,const std::string &id){
    if(named==nullptr&&id==""){
        return nullptr;
    }
    auto res=std::make_shared<storageReference>();
    res->transport=transport;
    res->named=named;
    res->id=id;
    return res;
    // return nullptr;
}
std::shared_ptr<storageReference>storageTransport::NewStoreReference(std::shared_ptr<Store_interface> store,std::shared_ptr<Named_interface> named,const std::string &id){
    auto s=std::make_shared<storageTransport>();
    s->store=store;
    s->defaultUIDMap=defaultUIDMap;
    s->defaultGIDMap=defaultGIDMap;
    return newReference(s,named,id);
}
void storageTransport::SetStore(std::shared_ptr<Store> store){
    this->store = store;
}
