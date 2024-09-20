#include "buildah/common.h"

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

}

std::vector<byte> retryCopyImage(
    std::shared_ptr<PolicyContext> policyContext,
    std::shared_ptr<ImageReference_interface> dest,
    std::shared_ptr<ImageReference_interface> src,
    std::shared_ptr<ImageReference_interface> registry,
    std::shared_ptr<copy::Options> copyOptions,
    int maxRetries,
    std::chrono::duration<int> retryDelay
){

}