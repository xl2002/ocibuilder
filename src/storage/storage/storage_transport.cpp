#include "storage/storage/storage_transport.h"
#include "utils/common/go/string.h"
#include "image/types/reference/normalize.h"
// const std::shared_ptr<StoreTransport_interface> Transport= std::make_shared<storageTransport>();

/**
 * @brief 获取存储传输的名称
 * @return std::string 返回存储传输的名称"containers-storage"
 */
std::string storageTransport::Name(){
    return "containers-storage";
}
/**
 * @brief 解析存储引用
 * @param store 存储接口指针
 * @param reference 要解析的引用字符串
 * @return std::shared_ptr<storageReference> 解析后的存储引用指针，失败返回nullptr
 */
std::shared_ptr<storageReference>storageTransport::ParseStoreReference(std::shared_ptr<Store_interface> store,const std::string &reference){
    auto img=store->Image(reference);
    std::string id;
    if(img!=nullptr&&hasPrefix(img->ID,reference)){ 
        id=img->ID;
    }
    std::shared_ptr<Named_interface> named=nullptr;
    named=ParseNormalizedNamed(reference);
    auto result=this->NewStoreReference(store,named,id);
    if(result!=nullptr){
        return result;
    }
    return nullptr;
}

// 将字符串转换为ImageReference
/**
 * @brief 将字符串转换为ImageReference
 * @param reference 要解析的引用字符串
 * @return std::shared_ptr<ImageReference_interface> 解析后的ImageReference指针
 */
std::shared_ptr<ImageReference_interface> storageTransport::ParseReference(const std::string &reference){

    return nullptr;
}

// 验证策略配置范围
/**
 * @brief 验证策略配置范围
 * @param scope 要验证的范围字符串
 */
void storageTransport::ValidatePolicyConfigurationScope(const std::string &scope){
    
}
/**
 * @brief 创建新的存储引用
 * @param transport 存储传输对象指针
 * @param named 命名接口指针
 * @param id 存储ID
 * @return std::shared_ptr<storageReference> 新创建的存储引用指针，参数无效时返回nullptr
 */
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
/**
 * @brief 创建新的存储引用
 * @param store 存储接口指针
 * @param named 命名接口指针
 * @param id 存储ID
 * @return std::shared_ptr<storageReference> 新创建的存储引用指针
 */
std::shared_ptr<storageReference>storageTransport::NewStoreReference(std::shared_ptr<Store_interface> store,std::shared_ptr<Named_interface> named,const std::string &id){
    auto s=std::make_shared<storageTransport>();
    s->store=store;
    s->defaultUIDMap=defaultUIDMap;
    s->defaultGIDMap=defaultGIDMap;
    return newReference(s,named,id);
}
/**
 * @brief 设置存储对象
 * @param store 要设置的存储对象指针
 */
void storageTransport::SetStore(std::shared_ptr<Store> store){
    this->store = store;
}
