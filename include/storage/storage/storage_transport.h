#if !defined(STORAGE_STORAGE_STORAGE_TRANSPORT_H)
#define STORAGE_STORAGE_STORAGE_TRANSPORT_H
#include <string>
#include <memory>
#include <map>
#include <vector>
// #include "image/transports/transports.h"
#include "storage/storage/storage.h"
#include "storage/storage/storage_reference.h"
#include "filesys/utils/idtools.h"
#include "image/types/types.h"
class storageReference;
class Store;
class Store_interface;
class ImageReference_interface;
class ImageTransport_interface {
public:
    virtual ~ImageTransport_interface()=default;
    // 返回传输名称
    virtual std::string Name()  = 0;
    // 将字符串转换为ImageReference
    virtual std::shared_ptr<ImageReference_interface> ParseReference(const std::string &reference)  = 0;
    // 验证策略配置范围
    virtual void ValidatePolicyConfigurationScope(const std::string &scope)  = 0;
};
class StoreTransport_interface:public ImageTransport_interface{
    public:
    virtual std::shared_ptr<storageReference>ParseStoreReference(std::shared_ptr<Store_interface> store,const std::string &reference)= 0;
    virtual std::shared_ptr<storageReference>NewStoreReference(std::shared_ptr<Store_interface> store,std::shared_ptr<Named_interface> named,const std::string &id)= 0;
};
class IDMap;
// class ImageReference_interface;
class storageTransport: public StoreTransport_interface{
    public:
    std::shared_ptr<Store_interface> store=nullptr;
    std::vector<IDMap> defaultUIDMap;
    std::vector<IDMap> defaultGIDMap;
    std::shared_ptr<storageReference>ParseStoreReference(std::shared_ptr<Store_interface> store,const std::string &reference)override;
        // 返回传输名称
    std::string Name() override;

    // 将字符串转换为ImageReference
    std::shared_ptr<ImageReference_interface> ParseReference(const std::string &reference) override;

    // 验证策略配置范围
    void ValidatePolicyConfigurationScope(const std::string &scope) override;

    std::shared_ptr<storageReference>NewStoreReference(std::shared_ptr<Store_interface> store,std::shared_ptr<Named_interface> named,const std::string &id) override;
    void SetStore(std::shared_ptr<Store> store);
};

extern std::shared_ptr<StoreTransport_interface> Transport;
std::shared_ptr<storageReference>newReference(std::shared_ptr<storageTransport> transport,std::shared_ptr<Named_interface> named,const std::string &id);


#endif // STORAGE_STORAGE_TRANSPORT_H)
