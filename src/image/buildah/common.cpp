#include "image/buildah/common.h"
#include "image/types/copy.h"
#include "image/buildah/retry.h"

std::shared_ptr<SystemContext> getSystemContext(
    std::shared_ptr<Store_interface> store,
    std::shared_ptr<SystemContext> defaults,
    std::string signaturePolicyPath
){
    auto sc=std::make_shared<SystemContext>();
    if(defaults!=nullptr){
        sc=defaults;
    }
    if(signaturePolicyPath!=""){
        sc->SignaturePolicyPath=signaturePolicyPath;
    }
    if(store!=nullptr){
        if(sc->SignaturePolicyPath=="" ){
            // auto userRegistriesFile

        }
    }
    return sc;
}
/**
 * @brief Get the Copy Options object
 * 
 * @param store 
 * @param reportWriter 
 * @param sourceSystemContext 
 * @param destinationSystemContext 
 * @param manifestType 
 * @param removeSignatures 
 * @param addSigner 
 * @param ociEncryptLayers 
 * @param ociEncryptConfig 
 * @param ociDecryptConfig 
 * @return std::shared_ptr<copy::Options> 
 */
std::shared_ptr<copy::Options> getCopyOptions(
    std::shared_ptr<Store_interface> store,
    std::ostream* reportWriter,
    std::shared_ptr<SystemContext> sourceSystemContext,
    std::shared_ptr<SystemContext> destinationSystemContext,
    std::string manifestType,
    bool removeSignatures,
    std::string addSigner,
    std::vector<int> ociEncryptLayers,
    std::shared_ptr<EncryptConfig> ociEncryptConfig,
    std::shared_ptr<DecryptConfig> ociDecryptConfig
){
    std::shared_ptr<SystemContext> sourceCtx = getSystemContext(store,nullptr,"");
    if(sourceSystemContext!=nullptr){
        sourceCtx = sourceSystemContext;
    }
    std::shared_ptr<SystemContext> destinationCtx = getSystemContext(store,nullptr,"");
    if(destinationSystemContext!=nullptr){
        destinationCtx = destinationSystemContext;
    }
    std::shared_ptr<copy::Options> options = std::make_shared<copy::Options>();
    options->reportWriter=reportWriter;
    options->sourceCtx=sourceCtx;
    options->destinationCtx=destinationCtx;
    options->forceManifestMIMEType=manifestType;
    options->removeSignatures=removeSignatures;
    options->signBy=addSigner;
    options->ociEncryptConfig=ociEncryptConfig;
    options->ociDecryptConfig=ociDecryptConfig;
    options->ociEncryptLayers=ociEncryptLayers;
    return options;
}
/**
 * @brief 将未压缩的镜像层传输到镜像库
 * 
 * @param policyContext 
 * @param dest 
 * @param src 
 * @param registry 
 * @param copyOptions 
 * @param maxRetries 
 * @param retryDelay 
 * @return std::vector<byte> 
 */
std::vector<byte> retryCopyImage(
    std::shared_ptr<PolicyContext> policyContext,
    std::shared_ptr<ImageReference_interface> dest,
    std::shared_ptr<ImageReference_interface> src,
    std::shared_ptr<ImageReference_interface> registry,
    std::shared_ptr<copy::Options> copyOptions,
    int maxRetries,
    std::chrono::duration<int> retryDelay
){
    std::vector<uint8_t> manifestBytes; // 等价于 Go 的 []byte
    std::exception_ptr err = nullptr;   // 等价于 Go 的 error
    std::exception_ptr lastErr = nullptr; // 等价于 Go 的 error
    auto operation = [&]() -> std::exception_ptr {
        try {
            manifestBytes = Image(policyContext, dest, src,registry ,copyOptions);
            if (registry && registry->Transport()->Name() != "docker") {
                lastErr = err;
                return nullptr; // 忽略非Docker传输的错误
            }
            return err;
        } catch (const std::exception& e) {
            return std::current_exception();
        }
    };
    // std::exception_ptr err = IfNecessary(operation, {maxRetries, retryDelay});
    auto retryOptions = std::make_shared<Retry::Options>(Retry::Options{maxRetries, retryDelay});

    auto retryError = IfNecessary(operation,retryOptions);
    if (lastErr) {
        err = lastErr;
    }

    return manifestBytes;
}