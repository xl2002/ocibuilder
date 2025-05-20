#include "cmd/build/imagebuildah/stage_executor.h"
#include "utils/common/error.h"
#include "filesys/system/rusage/rusage.h"
#include "filesys/system/rusage/rusage_unix.h"
#include "image/transports/transports.h"
#include "image/util/util.h"
#include "utils/common/go/string.h"
#include "image/buildah/commit.h"
#include <algorithm>
// #include "utils/logger/logrus/exported.h"
#include "image/image_types/docker/image.h"
#include "utils/logger/ProcessSafeLogger.h"

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
    auto execPtr=this->executor.lock();
    auto checkForLayers=execPtr->layers && execPtr->useCache;
    auto moreStages=index< this->stages->Stages.size()-1;
    auto lastStage=!moreStages;
    // auto onlyBaseImage=false;builder
    auto imageIsUsedLater=moreStages && (SetHas(execPtr->baseMap,stage->Name) || SetHas(execPtr->baseMap,std::to_string(stage->Position)));
    auto rootfsIsUsedLater=moreStages && (SetHas(execPtr->rootfsMap,stage->Name) || SetHas(execPtr->rootfsMap,std::to_string(stage->Position)));
    
    auto s=std::make_shared<Stages>();
    std::vector<Stage> st(stages->Stages.begin(),stages->Stages.begin()+index);
    s->Stages=st;
    auto ret=execPtr->waitForStage(base,s);
    if(ret.first && ret.second!="") {
        return std::make_tuple("",nullptr,false);
    }

    auto pullPolicy=execPtr->pullPolicy;
    std::unique_lock<std::mutex> lock(execPtr->stagesLock);
    bool preserveBaseImageAnnotationsAtStageStart=false;
    auto it = execPtr->imageMap.find(base);
    if(it!=execPtr->imageMap.end()){
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
            if(execPtr->rusageLogFile!=nullptr){
                std::cout<<FormatDiff(usage.first.Subtract(*resourceUsage));
            }
            resourceUsage=std::make_shared<Rusage>(usage.first);
        }
    };
    if(Supported()){
        auto resourceUsage=Get();
        if(resourceUsage.second){
            // return std::make_tuple("",std::shared_ptr<canonical>(nullptr),false);
            throw myerror(errorToString(resourceUsage.second));
        }

    }
    try{
        std::ignore=this->prepare(base,true,true,preserveBaseImageAnnotationsAtStageStart,pullPolicy);
    }catch(const myerror& e){
        throw;
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
        if(execPtr->quiet){
            log(commitMessage,std::vector<std::string>());
        }
    };

    auto logCachePulled=[&](std::string cacheKey, std::shared_ptr<named> remote){
        if(!execPtr->quiet){
            auto cachePullMessage = "--> Cache pulled from remote";
            std::cout << cachePullMessage <<" "<<remote->String() << ":" << cacheKey << std::endl;
        }
    };

    auto logCachePush=[&](std::string cacheKey){
        if(!execPtr->quiet){
            auto cachePushMessage = "--> Pushing cache";
            std::cout << cachePushMessage;
            for (auto &cache : execPtr->cacheTo) {
                std::cout << "[" << cache.String() << "]" ;
            }
            std::cout << ":" << cacheKey << std::endl;
        }
    };

    auto logCacheHit=[&](std::string cacheID){
        if(!execPtr->quiet){
            auto cacheHitMessage =  "--> Using cache";
            std::cout << cacheHitMessage << " " << cacheID << std::endl;
        }
    };

    auto logImageID=[&](std::string imgID){
        if(imgID.size()>12){
            imgID=imgID.substr(0,12);
        }
        if(execPtr->iidfile==""){
            std::cout << "--> " << imgID << std::endl;
        }
    };

    auto buildOutputOption=std::make_shared<BuildOutputOption>();
    auto canGenerateBuildOutput=execPtr->buildOutput !="" && lastStage;

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
            step->Resolve(node);//得修改from的处理逻辑
        }catch(const myerror& e){
            logger->log_error("resolving step "+std::to_string(i)+": "+std::string(e.what()));
            throw myerror("resolving step "+std::to_string(i)+": "+std::string(e.what()));
        }

        if(!execPtr->quiet){
            auto logMsg=step->Original;
            if(step->Heredocs.size()>0){

            }
            log(logMsg,std::vector<std::string>());
        }

        for(auto& flag:step->Flags){

        }
        auto noRunsRemaining=false;

        // if(moreInstructions){
        //     auto n=std::make_shared<Node>();
        //     n->Children.insert(n->Children.end(),node->Children.begin()+i+1,node->Children.end());
        //     noRunsRemaining=!ib->RequiresStart(n);
        // }
        if(!execPtr->layers){
            this->didExecute=true;
            try{
                // auto stage_p=std::make_shared<StageExecutor>(*this);
                ib->Run(step,shared_from_this(),noRunsRemaining);//将镜像层内容复制到overlay
            }catch(const myerror& e){
                throw myerror("building at STEP \""+step->Message+"\": "+std::string(e.what()));
            }
            auto addedContentSummary=getContentSummaryAfterAddingContent();
            if(moreInstructions){
                 // 获取当前时间
                auto timestamp = std::chrono::system_clock::now();
                if(execPtr->timestamp!=nullptr){
                    timestamp=*execPtr->timestamp;
                }
                this->builder->AddPrependedEmptyLayer(std::make_shared<std::chrono::system_clock::time_point>(timestamp),getCreatedBy(node,addedContentSummary),"","");
                continue;
            }else{
                if(lastStage || imageIsUsedLater){
                    logcommit(this->output,i);
                    auto c=this->commit(this->getCreatedBy(node,addedContentSummary),false,this->output,execPtr->squash,lastStage&& lastInstruction);
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
std::shared_ptr<Builder> StageExecutor::prepare(
    std::string from,
    bool initializeIBConfig,bool rebase,bool preserveBaseImageAnnotations,
    std::shared_ptr<PullPolicy> pullPolicy
){
    auto stage=this->stage;
    auto ib=stage->image_builder;
    auto node =stage->Node;
    auto execPtr=this->executor.lock();
    if(from==""){

    }
    auto displayFrom=from;
    auto asImageName=stage->Name;
    if(asImageName!=""){
        try {
            // std::atoi 返回 0 表示转换失败
            if (std::atoi(asImageName.c_str()) == 0 && asImageName != "0") {
                throw std::invalid_argument("Invalid integer conversion");
                logger->log_warning("Invalid integer conversion");
            }
        } catch (const std::invalid_argument&) {
            // 转换失败，执行下面的逻辑
            displayFrom = from + " AS " + asImageName;
        }
    }
    if(initializeIBConfig&&rebase){
        // Debugf("FROM %s\n",displayFrom.c_str());
        // if(!execPtr->quiet){
        //     this->log("FROM ",std::vector<std::string>({displayFrom}));
        // }
    }
    auto builderSystemContext=execPtr->systemContext;
    if(stage->image_builder->Platform!=""){

    }
    auto builderOptions=std::make_shared<BuilderOptions>();
    builderOptions->Args=ib->Args;
    builderOptions->FromImage=from;
    builderOptions->GroupAdd=execPtr->groupAdd;
    builderOptions->PullPolicy=pullPolicy;
    builderOptions->ContainerSuffix=execPtr->containerSuffix;
    builderOptions->Registry=execPtr->registry;
    builderOptions->BlobDirectory=execPtr->blobDirectory;
    builderOptions->SignaturePolicyPath=execPtr->signaturePolicyPath;
    builderOptions->ReportWriter=execPtr->reportWriter;
    builderOptions->SystemContext=builderSystemContext;
    builderOptions->Isolation=execPtr->isolation;
    builderOptions->NamespaceOptions->val=execPtr->namespaceOptions;
    builderOptions->ConfigureNetwork=execPtr->configureNetwork;
    builderOptions->CNIPluginPath=execPtr->cniPluginPath;
    builderOptions->CNIConfigDir=execPtr->cniConfigDir;
    builderOptions->NetworkInterface=execPtr->networkInterface;
    builderOptions->IDMappingOptions=execPtr->idmappingOptions;
    builderOptions->CommonBuildOpts=execPtr->commonBuildOptions;
    builderOptions->DefaultMountsFilePath=execPtr->defaultMountsFilePath;
    builderOptions->Format=execPtr->outputFormat;
    builderOptions->Capabilities=execPtr->capabilities;
    builderOptions->Devices=execPtr->devices;
    builderOptions->DeviceSpecs=execPtr->deviceSpecs;
    builderOptions->MaxPullRetries=execPtr->maxPullPushRetries;
    builderOptions->PullRetryDelay=execPtr->retryPullPushDelay;
    builderOptions->OciDecryptConfig=execPtr->ociDecryptConfig;
    //builderOptions->Logger=execPtr->logger;
    builderOptions->ProcessLabel=execPtr->processLabel;
    builderOptions->MountLabel=execPtr->mountLabel;
    builderOptions->PreserveBaseImageAnns=preserveBaseImageAnnotations;
    builderOptions->CDIConfigDir=execPtr->cdiConfigDir;
    std::shared_ptr<Builder> builder =NewBuilder(execPtr->store,builderOptions);
    if(execPtr->mountLabel==""&&execPtr->processLabel==""){
        execPtr->mountLabel=builder->MountLabel;
        execPtr->processLabel=builder->ProcessLabel;
    }
    if(initializeIBConfig){
        auto volumes=std::map<std::string, std::string>();
        for(auto it:builder->Volumes()){
            volumes[it]="";
        }
        auto ports=std::vector<Port>();
        for(auto p:builder->Ports()){
            Port tmpp(p);
            ports.push_back(tmpp);
        }
        auto dConfig=std::make_shared<container_Config>();
        dConfig->Hostname=builder->Hostname();
        dConfig->Domainname=builder->Domainname();
        dConfig->User=builder->User();
        dConfig->Env=builder->Env();
        dConfig->Cmd=builder->Cmd();
        dConfig->WorkingDir=builder->WorkDir();
        dConfig->Entrypoint=builder->Entrypoint();
        dConfig->Labels=builder->Labels();
        dConfig->Shell=builder->Shell();
        dConfig->StopSignal=builder->StopSignal();
        dConfig->OnBuild=builder->OnBuild();
        dConfig->Image=from;
        dConfig->Volumes=volumes;
        dConfig->ExposedPorts=ports;
        auto rootfs=std::make_shared<Docker::RootFS>();
        if(&builder->OCIv1->rootFS!=nullptr){
            rootfs->Type=builder->OCIv1->rootFS.type;
        }
        for(auto i:builder->OCIv1->rootFS.diffIDs){
            rootfs->Layers.push_back(i);
        }
        auto dImage=std::make_shared<Docker::Image>();
        dImage->Parent=builder->FromImageID;
        dImage->ContainerConfig=dConfig;
        dImage->Container=builder->Container;
        dImage->Architecture=builder->Architecture();
        dImage->Author=builder->Maintainer();
        dImage->RootFS=rootfs;
        dImage->Config=dImage->ContainerConfig;
        try{
            ib->FromImage(dImage,node);//空实现
            auto mountPoint=builder->Mount(builder->MountLabel);//空实现
            if(rebase){
                this->mountPoint=mountPoint;
                this->builder=builder;
            }
        }catch(const myerror& e){
            throw;
        }

    }
    return builder;
}


std::string StageExecutor::getContentSummaryAfterAddingContent(){
    auto digest=this->builder->ContentDigester->Digest();
    auto summary=digest.first;
    if(digest.second!=nullptr&&digest.second->digest!=""){
        summary=summary+":";
    }else{
        return "";
    }
    summary=summary+digest.second->Encoded();
    return summary;
}

// 拆分字符串为字段（类似strings.Fields）
std::vector<std::string> splitFields(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> fields;
    std::string field;
    while (iss >> field) {
        fields.push_back(field);
    }
    return fields;
}


/**
 * @brief 构建createdByz字符串
 * 
 * @param node 
 * @param addedContentSummary 
 * @return std::string 
 */
std::string StageExecutor::getCreatedBy(std::shared_ptr<Node> node,std::string addedContentSummary){
    if(node==nullptr){
        return "/bin/sh";
    }
    std::string upperValue = toUpper(node->Value);
    if(upperValue == "ARG"){
        for (const auto& variable : splitFields(node->Original)) {
            if (variable != "ARG") {
                argsFromContainerfile.push_back(variable);
            }
        }
        return "";
        // std::string buildArgs = getBuildArgsKey();
        // return "/bin/sh -c #(nop) ARG " + buildArgs;
    }else if(upperValue=="RUN"){
        // std::string shArg = "";
        // std::string buildArgs = getBuildArgsResolvedForRun();
        // if (node->Original.length() > 4) {
        //     shArg = node->Original.substr(4);
        // }
        // if (!buildArgs.empty()) {
        //     std::vector<std::string> buildArgsVec = splitFields(buildArgs);
        //     return "|" + std::to_string(buildArgsVec.size()) + " " + buildArgs + " /bin/sh -c " + shArg;
        // }
        // std::string result = "/bin/sh -c " + shArg;
        // if (!node->Heredocs.empty()) {
        //     for (const auto& doc : node->Heredocs) {
        //         std::string heredocContent = doc.Content;
        //         heredocContent.erase(std::remove_if(heredocContent.begin(), heredocContent.end(), ::isspace), heredocContent.end());
        //         result += "\n" + heredocContent;
        //     }
        // }
        // return result;
        return "";
    }else if (upperValue == "ADD" || upperValue == "COPY"){
        std::shared_ptr<Node> destination = node;
        while (destination->Next!= nullptr) {
            destination = destination->Next;
        }
        return "/bin/sh -c #(nop) " + upperValue + " " + addedContentSummary + " in " + destination->Value;
    } else {
        auto str=node->Original;
        str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
        return "/bin/sh -c #(nop) " + str;
    }

}
std::pair<std::string,std::shared_ptr<Canonical_interface>> StageExecutor::commit(
    std::string createdBy,
    bool emptyLayer,
    std::string output,
    bool squash,
    bool finalInstruction
){
    auto ib=this->stage->image_builder;
    auto execPtr=this->executor.lock();
    std::shared_ptr<ImageReference_interface> imageRef;
    if(output!=""){
        try{
            auto imageRef2=execPtr->resolveNameToImageRef(output);
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
    if(execPtr->architecture!=""){
        this->builder->SetArchitecture(execPtr->architecture);
    }
    if(execPtr->os!=""){
        this->builder->SetOS(execPtr->os);
    }
    if(execPtr->osVersion!=""){
        this->builder->SetOSVersion(execPtr->osVersion);
    }
    for(auto& osFeatureSpec:execPtr->osFeatures){
        
    }
    this->builder->SetUser(config->User);
    this->builder->ClearPorts();
    for(auto& p:config->ExposedPorts){
        this->builder->SetPorts(p.String());
    }
    for(auto& envSpec:config->Env){
        std::string key,val;
        std::tie(key,val,std::ignore)=Cut(envSpec,'=');
        this->builder->SetEnv(key,val);
    }
    for(auto& envSpec:execPtr->unsetEnvs){
        
    }
    this->builder->SetCmd(config->Cmd);
    // this->builder->ClearVolumes();
    for(auto& v:config->Volumes){
        this->builder->AddVolume(v.first,v.second);
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
        for(auto l:execPtr->layerLabels){
            std::string key,val;
            std::tie(key,val,std::ignore)=Cut(l,'=');
            this->builder->SetLabel(key,val);
        }
    }
    for(auto& v:config->Labels){
        this->builder->SetLabel(v.first,v.second);
    }
    // if(execPtr->commonBuildOptions->IdentityLabel==OptionalBool::OptionalBoolUndefined || execPtr->commonBuildOptions->IdentityLabel==OptionalBool::OptionalBoolTrue){
    //     this->builder->SetLabel(BuilderIdentityAnnotation,version);
    // }
    for(auto& l:execPtr->unsetLabels){
        
    }
    for(auto& annotationSpec:execPtr->annotations){
        std::string key,val;
        std::tie(key,val,std::ignore)=Cut(annotationSpec,'=');
        this->builder->SetAnnotation(key,val);
    }
    if(imageRef!=nullptr){
        auto logName=ImageName(imageRef);
        // std::cout<< "COMMIT "+logName<<std::endl;
    }else{
        // std::cout<< "COMMIT "+logName<<std::endl;
    }
    auto writer=execPtr->reportWriter;
    if(execPtr->layers ||!execPtr->useCache){
        writer=nullptr;
    }
    auto options=std::make_shared<CommitOptions>();
    options->Compression=execPtr->compression;
    options->outputimage=output;
    options->SignaturePolicyPath=execPtr->signaturePolicyPath;
    // options->ReportWriter=writer;
    options->PreferredManifestType=execPtr->outputFormat;
    options->SystemContext=execPtr->systemContext;
    options->Squash=squash;
    options->OmitHistory=execPtr->commonBuildOptions->OmitHistory;
    options->EmptyLayer=emptyLayer;
    options->BlobDirectory=execPtr->blobDirectory;
    options->SignBy=execPtr->signBy;
    options->MaxRetries=execPtr->maxPullPushRetries;
    options->RetryDelay=execPtr->retryPullPushDelay;
    options->HistoryTimestamp=execPtr->timestamp;
    options->Manifest=execPtr->manifest;
    options->check=execPtr->check;
    if(finalInstruction){
        options->ConfidentialWorkloadOptions=execPtr->confidentialWorkload;
        options->SBOMScanOptions=execPtr->sbomScanOptions;
    }
    std::string imgID;
    std::shared_ptr<Digest>manifestDigest;
    try{
        std::tie(imgID,std::ignore,manifestDigest)=this->builder->Commit(imageRef,options);
    }catch(const myerror& e){
        logger->log_error("Failed to commit changes: " + std::string(e.what()));
        throw;
    }
    std::shared_ptr<Canonical_interface> ref;
    if(imageRef!=nullptr){
        auto dref=imageRef->DockerReference();
        if(dref!=nullptr){
            try{
                ref=WithDigest(dref,manifestDigest);
            }catch(const myerror& e){
                logger->log_error("computing canonical reference for new image "+imgID+":"+std::string(e.what()));
                throw myerror("computing canonical reference for new image "+imgID+":"+std::string(e.what()));
            }
        }
    }
    // return std::make_pair(imgID,ref);
    return std::make_pair(manifestDigest->Encoded(),ref);
}

void StageExecutor::generateBuildOutput(std::shared_ptr<BuildOutputOption>buildOutputOpts){

}

void StageExecutor::UnrecognizedInstruction(std::shared_ptr<Step> step){

    return;
}
void StageExecutor::Preserve(std::string path){
    std::string mountpath;
    if(this->mountPoint==""){
        this->builder->MountPoint=this->builder->store->GetlayerStoragePath()+Separator+this->builder->container->LayerID;
        this->mountPoint=this->builder->MountPoint;
    }
    if(this->mountPoint!=""){
        mountpath=this->mountPoint;
    }else{
        mountpath=this->builder->MountPoint;
    }
    std::string volumePath=mountpath+"/diff"+path;
    auto path1=boost::filesystem::path(volumePath);
    // path.make_preferred();
    if(!boost::filesystem::exists(path1)){
        boost::filesystem::create_directories(path1);
    }
}
void StageExecutor::EnsureContainerPath(std::string path){

}

void StageExecutor::EnsureContainerPathAs(std::string path,std::string user,const mode_t* mode) {
    std::string mountpath=this->builder->store->GetlayerStoragePath()+Separator+this->builder->container->LayerID;
    auto workdirpath=boost::filesystem::path(mountpath+"/diff") / boost::filesystem::path(path);
    if(!boost::filesystem::exists(workdirpath)){
        boost::filesystem::create_directories(boost::filesystem::path(workdirpath));
        workdirpath.make_preferred();
    }
}
/**
 * @brief copy操作的实际功能函数
 * 
 * @param excludes 
 * @param copies 
 */
void StageExecutor::COPY(std::vector<std::string> excludes,std::vector<Copy> copies) {
    this->builder->ContentDigester->Restart();
    return this->performCopy(excludes,copies);
}
void StageExecutor::RUN(std::shared_ptr<Run> run) {

}
/**
 * @brief 进行Copy操作
 * 
 * @param excludes 
 * @param copies 
 */
void StageExecutor::performCopy(std::vector<std::string> excludes,std::vector<Copy> copies){
// volumeCacheInvalidate不写
    std::vector<Copy> copiesExtend;
    auto execPtr=this->executor.lock();
    for (size_t i = 0; i < copies.size(); ++i){
        Copy copy=copies[i];
        std::vector<string> sources;
        std::shared_ptr<IDMappingOptions> idMappingOptions = std::make_shared<IDMappingOptions>();
        std::vector<string> copyExcludes;
        bool stripSetuid = false;
        bool stripSetgid = false;
        bool preserveOwnership = false;
        std::string contextDir=execPtr->contextDir;
        
        //这里调试判断为false，直接跳过
        if(copy.Files.size()>0){

            if(copy.Src.size()>copy.Files.size()){
                
            }

        }
        // if(this->builder->MountPoint==""){
        if(copy.From=="from"){
            this->builder->MountPoint=this->builder->store->GetlayerStoragePath()+Separator+this->builder->FromImageID;

        }else{
            this->builder->MountPoint=this->builder->store->GetlayerStoragePath()+Separator+this->builder->container->LayerID;
            this->mountPoint=this->builder->MountPoint;
        }
        // }
        
        
        //这里调试判断为false，直接跳过
        if (copy.From.size()>0 && copy.Files.size()==0){

        }else{
            // 将 execPtr->excludes 的所有元素追加到 copyExcludes 中
            copyExcludes.insert(copyExcludes.end(), execPtr->excludes.begin(), execPtr->excludes.end());
            // 将 excludes 的所有元素追加到 copyExcludes 中
            copyExcludes.insert(copyExcludes.end(), excludes.begin(), excludes.end());
            stripSetuid = true;
			stripSetgid = true; 
        }

        for (size_t i = 0; i < copy.Src.size(); ++i){
            std::string src=copy.Src[i];
            //Source is a URL？不是

            // std::string joinedPath = joinPath(contextDir, src);
            appendSource(sources, src);
        }
        std::shared_ptr<AddAndCopyOptions> options = std::make_shared<AddAndCopyOptions>();
        options->Chmod=copy.Chmod;
        options->Chown=copy.Chown;
        options->Checksum=copy.Checksum;
        options->PreserveOwnership=preserveOwnership;
        options->ContextDir=contextDir;
        options->Excludes=copyExcludes;
        options->IgnoreFile=execPtr->ignoreFile;
        options->IDMappingOptions=idMappingOptions;
        options->StripSetuidBit=stripSetuid;
        options->StripSetgidBit=stripSetgid;

        if(copy.Files.size()>0){

        }
        this->builder->Add(copy.Dest,copy.Download,options,sources);
    }
    if(copiesExtend.size()>0){
        return this->performCopy(excludes,copiesExtend);
    }
    
}

// 模拟 Go 的 append 函数
void appendSource(std::vector<std::string>& sources, const std::string& path) {
    sources.push_back(path);
}