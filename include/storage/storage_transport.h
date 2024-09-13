#if !defined(STORAGE_STORAGE_TRANSPORT_H)
#define STORAGE_STORAGE_TRANSPORT_H
#include <string>
#include <memory>
#include <map>
#include <vector>
#include "transports/transports.h"
#include "storage/storage.h"
#include "storage/storage_reference.h"
#include "idtools/idtools.h"
class StoreTransport_interface:public ImageTransport_interface{
    public:
    virtual std::shared_ptr<storageReference>ParseStoreReference(std::shared_ptr<Store> store,const std::string &reference)= 0;
};

struct storageTransport: public StoreTransport_interface{
    std::shared_ptr<Store> store=nullptr;
    std::vector<IDMap> defaultUIDMap;
    std::vector<IDMap> defaultGIDMap;
    std::shared_ptr<storageReference>ParseStoreReference(std::shared_ptr<Store> store,const std::string &reference)override;
        // 返回传输名称
    std::string Name() override;

    // 将字符串转换为ImageReference
    std::shared_ptr<ImageReference_interface> ParseReference(const std::string &reference) override;

    // 验证策略配置范围
    void ValidatePolicyConfigurationScope(const std::string &scope) override;
};
extern const std::shared_ptr<StoreTransport_interface> Transport;

#endif // STORAGE_STORAGE_TRANSPORT_H)
