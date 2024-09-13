#if !defined(BUILDAH_COMMON_H)
#define BUILDAH_COMMON_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <chrono>
#include <iostream>
#include "define/types.h"
#include "storage/storage.h"
#include "copy/copy.h"
#include "signature/policy_eval.h"
#include "types/types.h"
std::shared_ptr<SystemContext> getSystemContext(
    std::shared_ptr<Store> store,
    std::shared_ptr<SystemContext> defaults,
    std::string signaturePolicyPath
);

std::shared_ptr<copy::Options> getCopyOptions(
    std::shared_ptr<Store> store,
    std::ostream* reportWriter,
    std::shared_ptr<SystemContext> sourceSystemContext,
    std::shared_ptr<SystemContext> destinationSystemContext,
    std::string manifestType,
    bool removeSignatures,
    std::string addSigner,
    std::vector<int> ociEncryptLayers,
    std::shared_ptr<EncryptConfig> ociEncryptConfig,
    std::shared_ptr<DecryptConfig> ociDecryptConfig
);

std::vector<byte> retryCopyImage(
    std::shared_ptr<PolicyContext> policyContext,
    std::shared_ptr<ImageReference_interface> dest,
    std::shared_ptr<ImageReference_interface> src,
    std::shared_ptr<ImageReference_interface> registry,
    std::shared_ptr<copy::Options> copyOptions,
    int maxRetries,
    std::chrono::duration<int> retryDelay
);


#endif // BUILDAH_COMMON_H)
