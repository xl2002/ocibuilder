#include "buildah/new.h"
#include "buildah/common.h"
#include "buildah/push.h"
#include "libimage/runtime.h"
#include "define/pull.h"
#include "libimage/pull.h"
std::shared_ptr<Builder> newBuilder(std::shared_ptr<Store> store,std::shared_ptr<BuilderOptions> options){
    std::shared_ptr<ImageReference_interface> ref;
    std::shared_ptr<storage::Image> img=std::make_shared<storage::Image>();
    if(options->FromImage=="scratch"){
        options->FromImage="";
    }
    if(options->NetworkInterface){
        //暂时搁置网络模块
        // options->NetworkInterface=getNetworkInterface();
    }
    auto systemContext=getSystemContext(store,options->SystemContext,options->SignaturePolicyPath);
    if(options->FromImage!="" && options->FromImage!="scratch"){
        std::shared_ptr<Runtime> imageRuntime;
        std::shared_ptr<PullPolicy> pullPolicy;
        try{
            auto runtimeOptions=std::make_shared<RuntimeOptions>();
            runtimeOptions->SystemContext=systemContext;
            imageRuntime=RuntimeFromStore(store,runtimeOptions);
            pullPolicy=ParsePullPolicy(options->PullPolicy->String());
        }catch(const myerror& e){
            throw;
        }
        auto pullOptions=std::make_shared<PullOptions>();
        pullOptions->RetryDelay=options->PullRetryDelay;
        pullOptions->OciDecryptConfig=options->OciDecryptConfig;
        pullOptions->SignaturePolicyPath=options->SignaturePolicyPath;
        pullOptions->Writer=options->ReportWriter;
        pullOptions->DestinationLookupReferenceFunc=cacheLookupReferenceFunc(options->BlobDirectory,std::make_shared<LayerCompression>(layerCompression::PreserveOriginal));
        auto maxRetries=(uint32_t)(options->MaxPullRetries);
        pullOptions->MaxRetries=std::make_shared<uint32_t>(maxRetries);
        try{
            auto pulledImages=imageRuntime->Pull(options->FromImage,pullPolicy,pullOptions);
            if(pulledImages.size()>0){

            }
        }catch(const myerror& e){
            throw;
        }
    }
}