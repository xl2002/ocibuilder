#include "image/buildah/buildah.h"
#include "utils/common/error.h"
#include "image/buildah/common.h"
#include "image/buildah/util.h"
#include "image/transports/transports.h"
#include "storage/storage/storage_transport.h"
#include "image/image_types/manifest/manifest.h"
#include "image/buildah/new.h"
// std::shared_ptr<PolicyTransportScopes> storageAllowedPolicyScopes=std::make_shared<PolicyTransportScopes>();
void Builder::Delete(){
    try{
        store->DeleteContainer(ContainerID);
    }catch(const myerror& e){
        throw;
    }
    MountPoint.clear();
    Container.clear();
    ContainerID.clear();
}

void Builder::AddPrependedEmptyLayer(std::shared_ptr<std::chrono::system_clock::time_point> created, std::string createdBy,std::string author, std::string comment){
    if(created!=nullptr){
        auto copiedTimestamp=*created;
        created=std::make_shared<std::chrono::system_clock::time_point>(copiedTimestamp);
    }
    History emptyLayer;
    emptyLayer.created=created;
    emptyLayer.createdBy=createdBy;
    emptyLayer.author=author;
    emptyLayer.comment=comment;
    emptyLayer.emptyLayer=true;
    this->PrependedEmptyLayers.push_back(emptyLayer);
}
void  Builder::SetCreatedBy(std::string how){
    this->ImageCreatedBy=how;
}
std::string  Builder::CreatedBy(){
    return this->ImageCreatedBy;
}
void  Builder::ClearAnnotations(){
    this->ImageAnnotations.clear();
}
std::string  Builder::OS(){
    return this->OCIv1->platform->OS;
}
void Builder::SetHostname(std::string name){
    this->Docker->config->Hostname=name;
}
void Builder::SetDomainname(std::string name){
    if(name!="" && this->Format!=Dockerv2ImageManifest){
        std::cout<<"DOMAINNAME is not supported for OCI image format, domainname "+name+ "will be ignored. Must use `docker` format"<<std::endl;
    }
    this->Docker->config->Domainname=name;
}
void Builder::SetUser(std::string spec){
    this->OCIv1->config->user=spec;
    this->Docker->config->User=spec;
}
void Builder::ClearPorts(){
    this->OCIv1->config->exposedPorts.clear();
    this->Docker->config->ExposedPorts.clear();
}
void Builder::SetEnv(std::string k,std::string v){
    auto reset=[&](std::vector<std::string>& s){
        std::vector<std::string> n;
        for(auto i=0;i<s.size();i++){
            if(hasPrefix(s[i],k+"=")){
                n.push_back(s[i]);
            }
        }
        n.push_back(k+"="+v);
        s=n;
    };
    reset(this->OCIv1->config->env);
    reset(this->Docker->config->Env);
}

void Builder::SetCmd(std::vector<std::string> cmd){
    this->OCIv1->config->cmd=cmd;
    this->Docker->config->Cmd=cmd;
}
void Builder::ClearVolumes(){
    this->OCIv1->config->volumes.clear();
    this->Docker->config->Volumes.clear();
}
void Builder::ClearOnBuild(){
    this->Docker->config->OnBuild.clear();
}
void Builder::SetWorkDir(std::string there){
    this->OCIv1->config->workingDir=there;
    this->Docker->config->WorkingDir=there;
}
void Builder::SetEntrypoint(std::vector<std::string> ep){
    this->OCIv1->config->entrypoint=ep;
    this->Docker->config->Entrypoint=ep;
}
void Builder::SetShell(std::vector<std::string> shell){
    if(!shell.empty() && this->Format!=Dockerv2ImageManifest){
        std::cout<<"SHELL is not supported for OCI image format, shell will be ignored. Must use `docker` format"<<std::endl;
    }
    this->Docker->config->Shell=shell;
}
void Builder::SetStopSignal(std::string sig){
    this->Docker->config->StopSignal=sig;
    this->OCIv1->config->stopSignal=sig;
}
void Builder::SetHealthcheck(std::shared_ptr<HealthConfig> config){
    this->Docker->config->Healthcheck=nullptr;
    if(config!=nullptr){
        if(this->Format!=Dockerv2ImageManifest){
            std::cout<<"HEALTHCHECK is not supported for OCI image format and will be ignored. Must use `docker` format"<<std::endl;
        }
        auto hc=std::make_shared<HealthConfig>();
        hc->Test=config->Test;
        hc->Interval=config->Interval;
        hc->Timeout=config->Timeout;
        hc->StartPeriod=config->StartPeriod;
        hc->Retries=config->Retries;
        this->Docker->config->Healthcheck=hc;
    }
}

void Builder::ClearLabels(){
    this->Docker->config->Labels.clear();
    this->OCIv1->config->labels.clear();
}
void Builder::SetLabel(std::string k,std::string v){
    this->OCIv1->config->labels[k]=v;
    this->Docker->config->Labels[k]=v;
}

std::tuple<std::string,std::shared_ptr<Canonical_interface>,std::shared_ptr<Digest>>  Builder::Commit(
    std::shared_ptr<ImageReference_interface> dest,
    std::shared_ptr<CommitOptions> options
){
    std::string imgID;
    std::shared_ptr<ImageReference_interface> src=nullptr;
    if(options->OmitTimestamp){
        
    }
    std::string nameToRemove="";
    if(dest==nullptr){
        
    }
    auto systemContext=getSystemContext(this->store,options->SystemContext,options->SignaturePolicyPath);
    bool blocked;
    try{
        blocked=isReferenceBlocked(dest,systemContext);
    }catch(const myerror& e){
        throw myerror("checking if committing to registry for "+ImageName(dest)+" is blocked: "+std::string(e.what()));
    }
    if(blocked){
        throw myerror("commit access to registry for"+ImageName(dest)+ "is blocked by configuration");
    }
    std::shared_ptr<Policy> commitPolicy;
    try{
        commitPolicy=DefaultPolicy(systemContext);
    }catch(const myerror& e){
        throw myerror("obtaining default signature policy: "+std::string(e.what()));
    }
    commitPolicy->Transports[Transport->Name()]=*storageAllowedPolicyScopes;
    auto policyContext=std::shared_ptr<PolicyContext>();
    try{
        policyContext=NewPolicyContext(commitPolicy);
    }catch(const myerror& e){
        throw myerror("creating new signature policy context: "+std::string(e.what()));
    }
    auto defer=[&](){
        try{
            policyContext->Destroy();
        }catch(const myerror& e){
            throw;
        }
    };
    bool insecure;
    try{
        insecure=checkRegistrySourcesAllows("commit to",dest);
    }catch(const myerror& e){
        throw;
    }
    if(insecure){
        
    }
    options->ExtraImageContent=copyStringStringMap(options->ExtraImageContent);
    std::map<std::string,std::string> extraImageContent, extraLocalContent;
    if(options->SBOMScanOptions.size()>0){
        
    }
    // std::shared_ptr<containerImageRef> src;
    try{
        src=this->makeContainerImageRef(options);
    }catch(const myerror& e){
        throw myerror("computing layer digests and building metadata for container "+this->ContainerID+":"+std::string(e.what()));
    }

    auto maybeCachedSrc=src;
    auto maybeCachedDest=dest;
    if(options->BlobDirectory!=""){

    }
    if(options->Compression->value==compression::Uncompressed){
        systemContext->OCIAcceptUncompressedLayers=true;
    }else if(options->Compression->value==compression::Gzip){
        systemContext->DirForceCompress=true;
    }
    if(systemContext->ArchitectureChoice!=this->Architecture()){
        systemContext->ArchitectureChoice=this->Architecture();
    }
    if(systemContext->OSChoice!=this->OS()){
        systemContext->OSChoice=this->OS();
    }

    std::vector<byte> manifestBytes;
    try{
        auto coptions=getCopyOptions(this->store, options->ReportWriter, nullptr, systemContext, "", false, options->SignBy, options->OciEncryptLayers, options->OciEncryptConfig, nullptr);
        manifestBytes=retryCopyImage(policyContext,maybeCachedDest,maybeCachedSrc,dest,coptions,options->MaxRetries,options->RetryDelay);
    }catch(const myerror& e){
        throw myerror("copying layers and metadata for container "+this->ContainerID+": "+std::string(e.what()));
    }
    if(dest->Transport()->Name()==Transport->Name()){
        std::shared_ptr<storage::Image>img;
        std::shared_ptr<ImageReference_interface>dest2=nullptr;
        try{
            std::tie(dest2,img)=ResolveReference(dest);
        }catch(const myerror& e){
            throw myerror("locating image "+ImageName(dest)+" in local storage: "+std::string(e.what()));
        }
        dest=dest2;
        imgID=img->ID;
        std::vector<std::string>toPruneNames(img->Names.size());
        for(auto& name:img->Names){
            if(nameToRemove!=""&& name.find(nameToRemove) != std::string::npos){
                toPruneNames.push_back(name);
            }
        }
        if(toPruneNames.size()>0){
            
        }
        if(options->IIDFile!=""){

        }
    }

    for(auto m:extraLocalContent){
        auto filename=m.first;
        auto content=m.second;
    }
    std::shared_ptr<Digest> manifestDigest;
    try{
        manifestDigest=manifest::Digest(manifestBytes);
    }catch(const myerror& e){
        throw myerror("computing digest of manifest of new image "+this->ContainerID+": "+std::string(e.what()));
    }
    std::shared_ptr<Canonical_interface> ref=nullptr;
    auto name=dest->DockerReference();
    if(name!=nullptr){
        try{
            ref=WithDigest(name,manifestDigest);
        }catch(const myerror& e){
            throw; 
        }
    }
    if(options->Manifest!=""){

    }
    defer();
    return std::make_tuple(imgID,ref,manifestDigest);
}

bool checkRegistrySourcesAllows(std::string forWhat,std::shared_ptr<ImageReference_interface> dest){
    
    return false;
}

std::shared_ptr<Builder> NewBuilder(std::shared_ptr<Store> store,std::shared_ptr<BuilderOptions> options){
    if(options->CommonBuildOpts==nullptr){
        options->CommonBuildOpts=std::make_shared<CommonBuildOptions>();
    }
    return newBuilder(store,options);
}