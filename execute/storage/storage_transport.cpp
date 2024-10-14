#include "storage/storage_transport.h"

const std::shared_ptr<StoreTransport> Transport= std::make_shared<storageTransport>();

std::shared_ptr<storageReference> storageTransport::ParseStoreReference(std::shared_ptr<store> store, const std::string &reference) const {
    // 函数实现
    return nullptr;
}

std::string storageTransport::Name() const {
    return "storageTransport";
}

std::shared_ptr<ImageReference> storageTransport::ParseReference(const std::string &reference) const {
    // 函数实现
    return nullptr;
}

void storageTransport::ValidatePolicyConfigurationScope(const std::string &scope) const {
    // 函数实现
}
