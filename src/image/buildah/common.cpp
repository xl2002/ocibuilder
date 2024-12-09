#include "image/buildah/common.h"

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
    return nullptr;
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
    return std::vector<byte>();
}