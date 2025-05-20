#if !defined(IMAGE_BUILDAH_COMMON_H)
#define IMAGE_BUILDAH_COMMON_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <chrono>
#include <iostream>
#include "image/types/define/types.h"
#include "storage/storage/storage.h"
#include "image/types/copy.h"
#include "image/types/signature/policy_eval.h"
#include "image/types/types.h"
#include "image/buildah/retry.h"

std::shared_ptr<SystemContext> getSystemContext(
    std::shared_ptr<Store_interface> store,
    std::shared_ptr<SystemContext> defaults,
    std::string signaturePolicyPath
);

std::shared_ptr<copy::Options> getCopyOptions(
    std::shared_ptr<Store_interface> store,
    std::shared_ptr<ostream> reportWriter,
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
