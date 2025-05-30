#include "image/buildah/buildah.h"
#include "utils/common/error.h"
#include "image/buildah/common.h"
#include "image/buildah/util.h"
#include "image/transports/transports.h"
#include "storage/storage/storage_transport.h"
#include "image/image_types/manifest/manifest.h"
#include "image/buildah/new.h"
#include "utils/common/json.h"
#include "image/image_types/v1/config.h"
#include "utils/logger/ProcessSafeLogger.h"
// std::shared_ptr<PolicyTransportScopes> storageAllowedPolicyScopes=std::make_shared<PolicyTransportScopes>();
/**
 * @brief 删除构建器关联的容器
 */
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

/**
 * @brief 添加一个前置的空层到镜像历史记录中
 * @param created 创建时间点
 * @param createdBy 创建者信息
 * @param author 作者信息
 * @param comment 注释信息
 */
void Builder::AddPrependedEmptyLayer(std::shared_ptr<std::chrono::system_clock::time_point> created, std::string createdBy,std::string author, std::string comment){
    if(created!=nullptr){
        auto copiedTimestamp=*created;
        created=std::make_shared<std::chrono::system_clock::time_point>(copiedTimestamp);
    }
    History emptyLayer;
    emptyLayer.created=*created;
    emptyLayer.createdBy=createdBy;
    emptyLayer.author=author;
    emptyLayer.comment=comment;
    emptyLayer.emptyLayer=true;
    this->PrependedEmptyLayers.push_back(emptyLayer);
}
/**
 * @brief 设置镜像创建者信息
 * @param how 创建者描述信息
 */
void  Builder::SetCreatedBy(std::string how){
    this->ImageCreatedBy=how;
}
/**
 * @brief 获取镜像创建者信息
 * @return 创建者描述信息
 */
std::string  Builder::CreatedBy(){
    return this->ImageCreatedBy;
}
/**
 * @brief 清除镜像的所有注解信息
 */
void  Builder::ClearAnnotations(){
    this->ImageAnnotations.clear();
}
/**
 * @brief 设置镜像的目标架构
 * @param arch 架构名称(如amd64, arm64等)
 */
void  Builder::SetArchitecture(std::string arch){
    this->OCIv1->platform.Architecture=arch;
}
/**
 * @brief 设置镜像维护者信息
 * @param who 维护者名称
 */
void Builder:: SetMaintainer(std::string who){
    for(auto & hi: this->OCIv1->history){
        hi.author=who;
    }
}
/**
 * @brief 获取镜像目标操作系统
 * @return 操作系统名称
 */
std::string  Builder::OS(){
    return this->OCIv1->platform.OS;
}
/**
 * @brief 设置镜像目标操作系统
 * @param os 操作系统名称
 */
void Builder::SetOS(std::string os){
    this->OCIv1->platform.OS=os;
}
/**
 * @brief 获取镜像操作系统版本
 * @return 操作系统版本字符串
 */
std::string Builder::OSVersion(){
    return this->OCIv1->platform.OSVersion;
}
/**
 * @brief 设置镜像操作系统版本
 * @param osVersion 操作系统版本字符串
 */
void Builder::SetOSVersion(std::string osVersion){
    this->OCIv1->platform.OSVersion=osVersion;
}
std::vector<std::string> Builder::Ports(){
    std::vector<std::string> p;
    for(auto k:this->OCIv1->config.exposedPorts){
        p.push_back(k.first);
    }
    return p;
}
void Builder::SetPorts(std::string port){
    this->OCIv1->config.exposedPorts[port]=boost::json::object();
}
/**
 * @brief 设置容器主机名
 * @param name 主机名
 */
void Builder::SetHostname(std::string name){
    this->Docker->config->Hostname=name;
}
/**
 * @brief 设置容器域名
 * @param name 域名
 */
void Builder::SetDomainname(std::string name){
    if(name!="" && this->Format!=Dockerv2ImageManifest){
        logger->log_warning("DOMAINNAME is not supported for OCI image format, domainname "+name+ "will be ignored. Must use `docker` format");
        std::cout<<"DOMAINNAME is not supported for OCI image format, domainname "+name+ "will be ignored. Must use `docker` format"<<std::endl;
    }
    this->Docker->config->Domainname=name;
}
/**
 * @brief 设置容器运行用户
 * @param spec 用户描述字符串
 */
void Builder::SetUser(std::string spec){
    this->OCIv1->config.user=spec;
    this->Docker->config->User=spec;
}
/**
 * @brief 清除所有暴露的端口设置
 */
void Builder::ClearPorts(){
    this->OCIv1->config.exposedPorts.clear();
    this->Docker->config->ExposedPorts.clear();
}
/**
 * @brief 设置容器环境变量
 * @param k 环境变量名
 * @param v 环境变量值
 */
void Builder::SetEnv(std::string k,std::string v){
    auto reset=[&](std::vector<std::string>& s){
        std::vector<std::string> n;
        for(auto i=0;i<s.size();i++){
            if(!hasPrefix(s[i],k+"=")){
                n.push_back(s[i]);
            }
        }
        n.push_back(k+"="+v);
        s=n;
    };
    reset(this->OCIv1->config.env);
    // reset(this->Docker->config->Env);
}


/**
 * @brief 设置容器默认命令
 * @param cmd 命令参数列表
 */
void Builder::SetCmd(std::vector<std::string> cmd){
    this->OCIv1->config.cmd=cmd;
    this->Docker->config->Cmd=cmd;
}

/**
 * @brief 清除所有卷挂载设置
 */
void Builder::ClearVolumes(){
    this->OCIv1->config.volumes.clear();
    this->Docker->config->Volumes.clear();
}
std::vector<std::string> Builder::Volumes(){
    std::vector<std::string> v;
    for(auto it:this->OCIv1->config.volumes){
        v.push_back(it.first);
    }
    return v;
}
void Builder::AddVolume(std::string k,std::string v){
    // this->OCIv1->config.volumes[k]=v;
    std::string volume;
    if(v==""){
        volume=k;
    }else{
        volume=k+":"+v;
    }
    // std::string volume=k+":"+v;
    
    this->OCIv1->config.volumes[volume]=boost::json::object();

}
/**
 * @brief 清除所有ONBUILD指令
 */
void Builder::ClearOnBuild(){
    this->Docker->config->OnBuild.clear();
}
/**
 * @brief 设置容器工作目录
 * @param there 工作目录路径
 */
void Builder::SetWorkDir(std::string there){
    this->OCIv1->config.workingDir=there;
    this->Docker->config->WorkingDir=there;
}
/**
 * @brief 设置容器入口点
 * @param ep 入口点参数列表
 */
void Builder::SetEntrypoint(std::vector<std::string> ep){
    this->OCIv1->config.entrypoint=ep;
    this->Docker->config->Entrypoint=ep;
}
/**
 * @brief 设置容器默认shell
 * @param shell shell路径列表
 */
void Builder::SetShell(std::vector<std::string> shell){
    if(!shell.empty() && this->Format!=Dockerv2ImageManifest){
        logger->log_warning("SHELL is not supported for OCI image format, shell will be ignored. Must use `docker` format");
        std::cout<<"SHELL is not supported for OCI image format, shell will be ignored. Must use `docker` format"<<std::endl;
    }
    this->Docker->config->Shell=shell;
}
/**
 * @brief 设置容器停止信号
 * @param sig 信号名称或编号
 */
void Builder::SetStopSignal(std::string sig){
    this->Docker->config->StopSignal=sig;
    this->OCIv1->config.stopSignal=sig;
}
/**
 * @brief 设置容器健康检查配置
 * @param config 健康检查配置对象
 */
void Builder::SetHealthcheck(std::shared_ptr<HealthConfig> config){
    this->Docker->config->Healthcheck=nullptr;
    if(config!=nullptr){
        if(this->Format!=Dockerv2ImageManifest){
            logger->log_warning("HEALTHCHECK is not supported for OCI image format and will be ignored. Must use `docker` format");
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

/**
 * @brief 清除所有标签设置
 */
void Builder::ClearLabels(){
    this->Docker->config->Labels.clear();
    this->OCIv1->config.labels.clear();
}
/**
 * @brief 设置容器标签
 * @param k 标签键
 * @param v 标签值
 */
void Builder::SetLabel(std::string k,std::string v){
    this->OCIv1->config.labels[k]=v;
    this->Docker->config->Labels[k]=v;
}

/**
 * @brief 提交容器为镜像
 * @param dest 目标镜像引用
 * @param options 提交选项
 * @return tuple包含: 镜像ID, 规范引用, 清单摘要
 */
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
        logger->log_error("checking if committing to registry for "+ImageName(dest)+" is blocked: "+std::string(e.what()));
        throw myerror("checking if committing to registry for "+ImageName(dest)+" is blocked: "+std::string(e.what()));
    }
    if(blocked){
        logger->log_error("commit access to registry for"+ImageName(dest)+ "is blocked by configuration");
        throw myerror("commit access to registry for"+ImageName(dest)+ "is blocked by configuration");
    }
    std::shared_ptr<Policy> commitPolicy;
    try{
        commitPolicy=DefaultPolicy(systemContext);
    }catch(const myerror& e){
        logger->log_error("obtaining default signature policy: "+std::string(e.what()));
        throw myerror("obtaining default signature policy: "+std::string(e.what()));
    }
    if(commitPolicy!=nullptr){
        commitPolicy->Transports[Transport->Name()]=*storageAllowedPolicyScopes;
    }
    auto policyContext=std::shared_ptr<PolicyContext>();
    try{
        policyContext=NewPolicyContext(commitPolicy);
    }catch(const myerror& e){
        logger->log_error("creating new signature policy context: "+std::string(e.what()));
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
        logger->log_error("computing layer digests and building metadata for container "+this->ContainerID+":"+std::string(e.what()));
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
        coptions->check=options->check;
        manifestBytes=retryCopyImage(policyContext,maybeCachedDest,maybeCachedSrc,dest,coptions,options->MaxRetries,options->RetryDelay);
    }catch(const myerror& e){
        logger->log_error("copying layers and metadata for container "+this->ContainerID+": "+std::string(e.what()));
        throw myerror("copying layers and metadata for container "+this->ContainerID+": "+std::string(e.what()));
    }
    // if(dest->Transport()->Name()==Transport->Name()){
    //     std::shared_ptr<storage::Image>img;
    //     std::shared_ptr<ImageReference_interface>dest2=nullptr;
    //     try{
    //         std::tie(dest2,img)=ResolveReference(dest);
    //     }catch(const myerror& e){
    //         throw myerror("locating image "+ImageName(dest)+" in local storage: "+std::string(e.what()));
    //     }
    //     dest=dest2;
    //     imgID=img->ID;
    //     std::vector<std::string>toPruneNames(img->Names.size());
    //     for(auto& name:img->Names){
    //         if(nameToRemove!=""&& name.find(nameToRemove) != std::string::npos){
    //             toPruneNames.push_back(name);
    //         }
    //     }
    //     if(toPruneNames.size()>0){
            
    //     }
    //     if(options->IIDFile!=""){

    //     }
    // }
    //得到imgID，为镜像的config的digest
    auto manifest=unmarshal<::Manifest>(vectorToString(manifestBytes));
    imgID=manifest.Config.Digests.Encoded();
    for(auto m:extraLocalContent){
        auto filename=m.first;
        auto content=m.second;
    }
    std::shared_ptr<Digest> manifestDigest;
    try{
        manifestDigest=manifest::Digest(manifestBytes);
    }catch(const myerror& e){
        logger->log_error("computing digest of manifest of new image "+this->ContainerID+": "+std::string(e.what()));
        throw myerror("computing digest of manifest of new image "+this->ContainerID+": "+std::string(e.what()));
    }
    std::shared_ptr<Canonical_interface> ref=nullptr;
    // auto name=dest->DockerReference();
    // if(name!=nullptr){
    //     try{
    //         ref=WithDigest(name,manifestDigest);
    //     }catch(const myerror& e){
    //         throw;
    //     }
    // }
    if(options->Manifest!=""){

    }
    defer();
    return std::make_tuple(imgID,ref,manifestDigest);
}

/**
 * @brief 检查是否允许访问指定注册表
 * @param forWhat 操作描述
 * @param dest 目标镜像引用
 * @return 是否允许访问
 */
bool checkRegistrySourcesAllows(std::string forWhat,std::shared_ptr<ImageReference_interface> dest){
    
    return false;
}

/**
 * @brief 创建新的构建器实例
 * @param store 存储接口
 * @param options 构建选项
 * @return 新的构建器实例
 */
std::shared_ptr<Builder> NewBuilder(std::shared_ptr<Store> store,std::shared_ptr<BuilderOptions> options){
    if(options->CommonBuildOpts==nullptr){
        options->CommonBuildOpts=std::make_shared<CommonBuildOptions>();
    }
    return newBuilder(store,options);
}
