#include "storage/storage_transport.h"

const std::shared_ptr<StoreTransport_interface> Transport= std::make_shared<storageTransport>();

std::string storageTransport::Name(){
    return "containers-storage";
}
std::shared_ptr<storageReference>storageTransport::ParseStoreReference(std::shared_ptr<Store_interface> store,const std::string &reference){

    return nullptr;
}

// 将字符串转换为ImageReference
std::shared_ptr<ImageReference_interface> storageTransport::ParseReference(const std::string &reference){

    return nullptr;
}

// 验证策略配置范围
void storageTransport::ValidatePolicyConfigurationScope(const std::string &scope){
    
}