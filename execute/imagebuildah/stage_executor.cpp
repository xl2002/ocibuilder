#include "imagebuildah/stage_executor.h"
#include "cobra/error.h"
#include "rusage/rusage.h"
#include "rusage/rusage_unix.h"
#include "transports/transports.h"
#include "util/util.h"
#include "go/string.h"
#include "buildah/commit.h"
#include <algorithm>
void StageExecutor::Delete(){
    if(builder!=nullptr){
        try{
            builder->Delete();
            builder=nullptr;
        }catch(const myerror& e){
            throw;
        }
    }
}

std::tuple<std::string,std::shared_ptr<Canonical_interface>,bool> StageExecutor::Execute(std::string base){
    //返回值变量
    std::string imgID="";
    std::shared_ptr<Canonical_interface> ref=nullptr;
    bool onlyBaseImage=false;

    std::shared_ptr<Rusage> resourceUsage;
    auto ib=stage->image_builder;
    auto checkForLayers=executor->layers && executor->useCache;
    auto moreStages=index< this->stages->Stages.size()-1;
    auto lastStage=!moreStages;
    // auto onlyBaseImage=false;
    auto imageIsUsedLater=moreStages && (SetHas(this->executor->baseMap,stage->Name) || SetHas(this->executor->baseMap,std::to_string(stage->Position)));
    auto rootfsIsUsedLater=moreStages && (SetHas(this->executor->rootfsMap,stage->Name) || SetHas(this->executor->rootfsMap,std::to_string(stage->Position)));
    
    auto s=std::make_shared<Stages>();
    std::vector<Stage> st(stages->Stages.begin(),stages->Stages.begin()+index);
    s->Stages=st;
    auto ret=this->executor->waitForStage(base,s);
    if(ret.first && ret.second!="") {
        return std::make_tuple("",nullptr,false);
    }

    auto pullPolicy=this->executor->pullPolicy;
    std::unique_lock<std::mutex> lock(this->executor->stagesLock);
    bool preserveBaseImageAnnotationsAtStageStart;
    auto it = this->executor->imageMap.find(base);
    if(it!=this->executor->imageMap.end()){
        base=it->second;
        pullPolicy->value=Pull_Policy::PullNever;
        preserveBaseImageAnnotationsAtStageStart=true;
    }
    lock.unlock();

    std::function<void()> logRusage=[&](){
        if(Supported()){
            auto usage=Get();
            if(usage.second){
                throw myerror(errorToString(usage.second));
            }
            if(this->executor->rusageLogFile!=nullptr){
                *this->executor->rusageLogFile<<FormatDiff(usage.first.Subtract(*resourceUsage));
            }
            resourceUsage=std::shared_ptr<Rusage>(&usage.first);
        }
    };

    if(Supported()){
        auto resourceUsage=Get();
        if(resourceUsage.second){
            // return std::make_tuple("",std::shared_ptr<canonical>(nullptr),false);
            throw myerror(errorToString(resourceUsage.second));
        }

    }

    auto children=stage->Node->Children;
    std::function<void(std::string,int)> logcommit=[&](std::string output,int instruction){
        auto moreInstructions=instruction<children.size()-1;
        if(moreInstructions || moreStages){
            return;
        }
        std::string commitMessage="COMMIT";
        if(output!=""){
            commitMessage=commitMessage+" "+output;
        }
        if(this->executor->quiet){
            log(commitMessage,std::vector<std::string>());
        }
    };

    auto logCachePulled=[&](std::string cacheKey, std::shared_ptr<named> remote){
        if(!this->executor->quiet){
            auto cachePullMessage = "--> Cache pulled from remote";
            *this->executor->out << cachePullMessage <<" "<<remote->String() << ":" << cacheKey << std::endl;
        }
    };

    auto logCachePush=[&](std::string cacheKey){
        if(!this->executor->quiet){
            auto cachePushMessage = "--> Pushing cache";
            *this->executor->out << cachePushMessage;
            for (auto &cache : this->executor->cacheTo) {
                *this->executor->out << "[" << cache.String() << "]" ;
            }
            *this->executor->out << ":" << cacheKey << std::endl;
        }
    };

    auto logCacheHit=[&](std::string cacheID){
        if(!this->executor->quiet){
            auto cacheHitMessage =  "--> Using cache";
            *this->executor->out << cacheHitMessage << " " << cacheID << std::endl;
        }
    };

    auto logImageID=[&](std::string imgID){
        if(imgID.size()>12){
            imgID=imgID.substr(0,12);
        }
        if(this->executor->iidfile==""){
            *this->executor->out << "--> " << imgID << std::endl;
        }
    };

    auto buildOutputOption=std::make_shared<BuildOutputOption>();
    auto canGenerateBuildOutput=this->executor->buildOutput !="" && lastStage;

    if(canGenerateBuildOutput){

    }

    if(children.size()==0){
        logcommit(this->output,-1);
        onlyBaseImage=true;
        if(canGenerateBuildOutput){
            
        }
        logImageID(imgID);
    }

    for(auto i=0;i<children.size();i++){
        auto node=children[i];
        logRusage();
        auto moreInstructions= i<children.size()-1;
        auto lastInstruction=!moreInstructions;
        auto step=ib->Step_new();
        try{
            step->Resolve(node);
        }catch(const myerror& e){
            throw myerror("resolving step "+std::to_string(i)+": "+std::string(e.what()));
        }

        if(!this->executor->quiet){
            auto logMsg=step->Original;
            if(step->Heredocs.size()>0){

            }
            log(logMsg,std::vector<std::string>());
        }

        for(auto& flag:step->Flags){

        }
        auto noRunsRemaining=false;

        if(moreInstructions){
            auto n=std::make_shared<Node>();
            n->Children.insert(n->Children.end(),node->Children.begin()+i+1,node->Children.end());
            noRunsRemaining=!ib->RequiresStart(n);
        }
        if(!this->executor->layers){
            this->didExecute=true;
            try{
                std::shared_ptr<StageExecutor>stage_p(this);
                ib->Run(step,stage_p,noRunsRemaining);
            }catch(const myerror& e){
                throw myerror("building at STEP \""+step->Message+"\": "+std::string(e.what()));
            }
            auto addedContentSummary=getContentSummaryAfterAddingContent();
            if(moreInstructions){
                 // 获取当前时间
                auto timestamp = std::chrono::system_clock::now();
                if(this->executor->timestamp!=nullptr){
                    timestamp=*this->executor->timestamp;
                }
                this->builder->AddPrependedEmptyLayer(std::make_shared<std::chrono::system_clock::time_point>(timestamp),getCreatedBy(node,addedContentSummary),"","");
                continue;
            }else{
                if(lastStage || imageIsUsedLater){
                    logcommit(this->output,i);
                    auto c=this->commit(this->getCreatedBy(node,addedContentSummary),false,this->output,this->executor->squash,lastStage&& lastInstruction);
                    imgID=c.first;
                    ref=c.second;
                    logImageID(imgID);
                    if(canGenerateBuildOutput){
                        try{
                            this->generateBuildOutput(buildOutputOption);
                        }catch(const myerror& e){
                            throw;
                        }
                    }
                }else{
                    imgID="";
                }
                break;
            }
        }
        // this->didExecute=false;
    }
    logRusage();
    return std::make_tuple(imgID,ref,onlyBaseImage);
}


std::string StageExecutor::getContentSummaryAfterAddingContent(){
    auto digest=this->builder->ContentDigester->Digest();
    auto summary=digest.first;
    if(digest.second->digest!=""){
        summary=summary+":";
    }
    summary=summary+digest.second->Encoded();
    return summary;
}

std::string StageExecutor::getCreatedBy(std::shared_ptr<Node> node,std::string addedContentSummary){
    
    return "";
}
std::pair<std::string,std::shared_ptr<Canonical_interface>> StageExecutor::commit(
    std::string createdBy,
    bool emptyLayer,
    std::string output,
    bool squash,
    bool finalInstruction
){
    auto ib=this->stage->image_builder;
    std::shared_ptr<ImageReference_interface> imageRef;
    if(output!=""){
        try{
            auto imageRef2=this->executor->resolveNameToImageRef(output);
            imageRef=imageRef2;
        }catch(const myerror& e){
            throw;
        }
    }
    if(ib->Author!=""){

    }
    auto config=ib->Config();
    if(createdBy!=""){
        this->builder->SetCreatedBy(createdBy);
    }
    this->builder->SetHostname(config->Hostname);
    this->builder->SetDomainname(config->Domainname);
    if(this->executor->os!=""){
        
    }
    if(this->executor->osVersion!=""){
        
    }
    for(auto& osFeatureSpec:this->executor->osFeatures){
        
    }
    this->builder->SetUser(config->User);
    this->builder->ClearPorts();
    for(auto& p:config->ExposedPorts){
        
    }
    for(auto& envSpec:config->Env){
        std::string key,val;
        std::tie(key,val,std::ignore)=Cut(envSpec,'=');
        this->builder->SetEnv(key,val);
    }
    for(auto& envSpec:this->executor->unsetEnvs){
        
    }
    this->builder->SetCmd(config->Cmd);
    this->builder->ClearVolumes();
    for(auto& v:config->Volumes){
        
    }
    this->builder->ClearOnBuild();
    for(auto& onBuildSpec:config->OnBuild){
        
    }
    this->builder->SetWorkDir(config->WorkingDir);
    this->builder->SetEntrypoint(config->Entrypoint);
    this->builder->SetShell(config->Shell);
    this->builder->SetStopSignal(config->StopSignal);
    if(config->Healthcheck!=nullptr){
        
    }else{
        this->builder->SetHealthcheck(nullptr);
    }
    this->builder->ClearLabels();
    if(output==""){
        
    }
    for(auto& v:config->Labels){
        
    }
    if(this->executor->commonBuildOptions->IdentityLabel==OptionalBool::OptionalBoolUndefined || this->executor->commonBuildOptions->IdentityLabel==OptionalBool::OptionalBoolTrue){
        this->builder->SetLabel(BuilderIdentityAnnotation,version);
    }
    for(auto& l:this->executor->unsetLabels){
        
    }
    for(auto& annotationSpec:this->executor->annotations){

    }
    if(imageRef!=nullptr){
        auto logName=ImageName(imageRef);
        // std::cout<< "COMMIT "+logName<<std::endl;
    }else{
        // std::cout<< "COMMIT "+logName<<std::endl;
    }
    auto writer=this->executor->reportWriter;
    if(this->executor->layers ||!this->executor->useCache){
        writer=nullptr;
    }
    auto options=std::make_shared<CommitOptions>();
    options->Compression=this->executor->compression;
    options->SignaturePolicyPath=this->executor->signaturePolicyPath;
    options->ReportWriter=writer;
    options->PreferredManifestType=this->executor->outputFormat;
    options->SystemContext=this->executor->systemContext;
    options->Squash=squash;
    options->OmitHistory=this->executor->commonBuildOptions->OmitHistory;
    options->EmptyLayer=emptyLayer;
    options->BlobDirectory=this->executor->blobDirectory;
    options->SignBy=this->executor->signBy;
    options->MaxRetries=this->executor->maxPullPushRetries;
    options->RetryDelay=this->executor->retryPullPushDelay;
    options->HistoryTimestamp=this->executor->timestamp;
    options->Manifest=this->executor->manifest;
    if(finalInstruction){
        options->ConfidentialWorkloadOptions=this->executor->confidentialWorkload;
        options->SBOMScanOptions=this->executor->sbomScanOptions;
    }
    std::string imgID;
    std::shared_ptr<Digest>manifestDigest;
    try{
        std::tie(imgID,std::ignore,manifestDigest)=this->builder->Commit(imageRef,options);
    }catch(const myerror& e){
        throw;
    }
    std::shared_ptr<Canonical_interface> ref;
    if(imageRef!=nullptr){
        auto dref=imageRef->DockerReference();
        if(dref!=nullptr){
            try{
                ref=WithDigest(dref,manifestDigest);
            }catch(const myerror& e){
                throw myerror("computing canonical reference for new image "+imgID+":"+std::string(e.what()));
            }
        }
    }
    return std::make_pair(imgID,ref);
}

void StageExecutor::generateBuildOutput(std::shared_ptr<BuildOutputOption>buildOutputOpts){

}

void StageExecutor::UnrecognizedInstruction(std::shared_ptr<Step> step){

    return;
}
void StageExecutor::Preserve(std::string path){

}
void StageExecutor::EnsureContainerPath(std::string path){

}
void StageExecutor::EnsureContainerPathAs(std::string path,std::string user,const mode_t* mode) {

}
void StageExecutor::COPY(std::vector<std::string> excludes,std::vector<Copy> copies) {

}
void StageExecutor::RUN(std::shared_ptr<Run> run) {

}