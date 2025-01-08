#include "image/buildah/new.h"
#include "image/buildah/common.h"
#include "image/buildah/push.h"
#include "image/libimage/runtime.h"
#include "image/types/define/pull.h"
#include "image/libimage/pull.h"
#include "image/image_types/manifest/manifest.h"
#include "image/types/define/types.h"
#include "image/image_types/v1/annotations.h"
#include <boost/filesystem.hpp>

std::shared_ptr<Builder> newBuilder(std::shared_ptr<Store> store,std::shared_ptr<BuilderOptions> options){
    std::shared_ptr<ImageReference_interface> ref=nullptr;
    std::shared_ptr<storage::Image> img=std::make_shared<storage::Image>();
    // if(options->FromImage=="scratch"){
    //     options->FromImage="";
    // }
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
                img=pulledImages[0]->storageImage;
                // ref=pulledImages[0]->StorageReference();
            }
        }catch(const myerror& e){
            throw;
        }
    }
    auto imageSpec=options->FromImage;
    std::string imageID;
    std::string imageDigest;
    std::string topLayer;
    if(img!=nullptr){
        imageID=img->ID;
        topLayer=img->TopLayer;
    }
    std::shared_ptr<Image_interface> src=nullptr;
    if(ref!=nullptr){
        try{
            auto srcSrc=ref->NewImageSource(systemContext);
            std::vector<uint8_t> manifestBytes;
            std::string manifestType;
            std::tie(manifestBytes,manifestType)=srcSrc->GetManifest(nullptr);
            auto manifestDigest=manifest::Digest(manifestBytes);
            if(manifestDigest!=nullptr){
                imageDigest=manifestDigest->String();
            }
            std::shared_ptr<::Digest> instanceDigest=nullptr;
            // src=FromUnparsedImage(systemContext,UnparsedInstance(srcSrc,instanceDigest));
        }catch(const myerror& e){
            throw;
        }
    }
    std::shared_ptr<Container> container;
    std::string tmpName="working-container";
    auto coptions=std::make_shared<ContainerOptions>();
    coptions->labelOpts=options->CommonBuildOpts->LabelOpts;

    container=store->CreateContainer("",{tmpName},imageID,"","",coptions);
    
    auto builder=std::make_shared<Builder>();
    builder->store=store;
    builder->Type=Package+version;
    builder->FromImage=imageSpec;
    builder->FromImageID=imageID;
    builder->FromImageDigest=imageDigest;
    builder->GroupAdd=options->GroupAdd;
    builder->Container=tmpName;
    builder->ContainerID=container->ID;
    builder->container=container;
    builder->DefaultMountsFilePath=options->DefaultMountsFilePath;
    builder->Isolation=options->Isolation;
    builder->ConfigureNetwork=options->ConfigureNetwork;
    builder->CNIPluginPath=options->CNIPluginPath;
    builder->CNIConfigDir=options->CNIConfigDir;
    builder->Capabilities=options->Capabilities;
    builder->CommonBuildOpts=options->CommonBuildOpts;
    builder->TopLayer=topLayer;
    builder->Args=options->Args;
    builder->Format=options->Format;
    builder->Devices=options->Devices;
    builder->NetworkInterface=options->NetworkInterface;
    builder->CDIConfigDir=options->CDIConfigDir;
    
    try{
        if(!boost::filesystem::exists(imageSpec)){
            builder->initConfig(src,systemContext);
        }
    }catch(const myerror & e){
        throw;
    }
    if(!options->PreserveBaseImageAnns){
        // builder->SetAnnotation(AnnotationBaseImageDigest,imageDigest);
        builder->SetAnnotation(AnnotationBaseImageName,imageSpec);
    }
    return builder;
}