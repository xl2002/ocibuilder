#if !defined(STORAGE_STORAGE_TRANSPORT_H)
#define STORAGE_STORAGE_TRANSPORT_H
#include <string>
#include <memory>
#include <map>
#include <vector>
#include "transports/transports.h"
#include "storage/storage.h"
#include "storage/storage_reference.h"
class StoreTransport:public ImageTransport{
    public:
    virtual std::shared_ptr<storageReference>ParseStoreReference(std::shared_ptr<store> store,const std::string &reference) const= 0;
    virtual ~StoreTransport() = default;
};

struct storageTransport: public StoreTransport{
    public:
    ~storageTransport() override = default;
    
    std::shared_ptr<storageReference>ParseStoreReference(std::shared_ptr<store> store,const std::string &reference) const override;
        // 返回传输名称
    std::string Name() const override;

    // 将字符串转换为ImageReference
    std::shared_ptr<ImageReference> ParseReference(const std::string &reference) const override;

    // 验证策略配置范围
    void ValidatePolicyConfigurationScope(const std::string &scope) const override;
};
extern const std::shared_ptr<StoreTransport> Transport;

#endif // STORAGE_STORAGE_TRANSPORT_H)
