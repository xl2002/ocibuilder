#include "buildah/buildah.h"
#include "cobra/error.h"
#include "buildah/common.h"
#include "buildah/util.h"
#include "transports/transports.h"
#include "storage/storage_transport.h"
#include "manifest/manifest.h"
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
    std::shared_ptr<ImageReference_interface> src;
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
        std::shared_ptr<ImageReference_interface>dest2;
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
        manifestDigest=manifest::Digest_trans(manifestBytes);
    }catch(const myerror& e){
        throw myerror("computing digest of manifest of new image "+this->ContainerID+": "+std::string(e.what()));
    }
    std::shared_ptr<Canonical_interface> ref;
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



















void fixupConfig(std::shared_ptr<Builder> b, const std::shared_ptr<SystemContext>& sys) {
    try {
        // Prefer image-level settings over those from the container it was built from.
        if (b->Docker && b->Docker->config) {
            b->Docker->ContainerConfig = b->Docker->config;  // 解引用shared_ptr
        }
        // 如果ContainerConfig为空，则初始化为新的config对象
        if (!b->Docker->ContainerConfig) {
            b->Docker->ContainerConfig = std::make_shared<container_Config>();
        }
        b->Docker->config = std::make_shared<container_Config>(*b->Docker->ContainerConfig); // 复制ContainerConfig

        b->Docker->DockerVersion = "";

        // 使用boost获取当前UTC时间
        auto now = std::chrono::system_clock::now();

        // 如果Created为空，则设置当前时间
        if (b->Docker->Created.time_since_epoch().count() == 0) {
            b->Docker->Created = now;
        }

        // 如果OCIv1的Created为空，则设置当前时间
        if (!b->OCIv1 || (b->OCIv1->created && b->OCIv1->created->time_since_epoch().count() == 0)) {
            b->OCIv1->created = std::make_shared<std::chrono::system_clock::time_point>(now);
        }

        // 如果没有设置OS，基于系统上下文或环境变量设置
        if (b->OS().empty()) {
            if (sys && !sys->OSChoice.empty()) {
                b->SetOS(sys->OSChoice);
            } else {
                b->SetOS(boost::process::environment()["OSTYPE"]);  // 使用boost::process获取操作系统信息
            }
        }

        // 如果没有设置Architecture，基于系统上下文或环境变量设置
        if (b->Architecture().empty()) {
            if (sys && !sys->ArchitectureChoice.empty()) {
                b->SetArchitecture(sys->ArchitectureChoice);
            } else {
                b->SetArchitecture(boost::process::environment()["HOSTTYPE"]);  // 使用boost::process获取体系结构信息
            }

            // 归一化架构和变体
            auto ps = internalUtil::NormalizePlatform(b->OS(), b->Architecture(), b->Variant());
            b->SetArchitecture(ps.Architecture);
            b->SetVariant(ps.Variant);
        }

        // 如果没有设置Variant，则检查是否在系统上下文中定义
        if (b->Variant().empty()) {
            if (sys && !sys->VariantChoice.empty()) {
                b->SetVariant(sys->VariantChoice);
            }

            // 归一化架构和变体
            auto ps = internalUtil::NormalizePlatform(b->OS(), b->Architecture(), b->Variant());
            b->SetArchitecture(ps.Architecture);
            b->SetVariant(ps.Variant);
        }

        // 如果Docker格式并且没有设置主机名，生成随机ID作为主机名
        if (b->Format == "docker" && b->Hostname().empty()) {
            b->SetHostname(generateRandomID());
        }

    } catch (const myerror& e) {
        // 处理myerror类型的异常
        std::cerr << "Error: " << e.what() << std::endl;
        throw;

    } catch (const std::exception& e) {
        // 将其他异常转换为myerror重新抛出
        throw myerror("fixupConfig中的未知错误: " + std::string(e.what()));
    }
}
// OpenBuilder 函数的 C++ 实现
std::shared_ptr<Builder> OpenBuilder(std::shared_ptr<store> store, const std::string& container) {
    std::shared_ptr<Builder> b = std::make_shared<Builder>();
    
    try {
        // 获取构建容器的目录
        boost::filesystem::path cdir = store->ContainerDirectory(container);

        // 读取构建容器的状态文件
        boost::filesystem::path stateFilePath = cdir / "state.json";  // 假设stateFile是 "state.json"
        if (!boost::filesystem::exists(stateFilePath)) {
            throw myerror("找不到状态文件: " + stateFilePath.string());
        }

        // 读取状态文件内容
        boost::filesystem::ifstream stateFileStream(stateFilePath);
        std::string buildstate((std::istreambuf_iterator<char>(stateFileStream)),
                               std::istreambuf_iterator<char>());
        stateFileStream.close();

        // 解析JSON数据到Builder对象
        boost::json::value parsedState = boost::json::parse(buildstate);
        b = boost::json::value_to<std::shared_ptr<Builder>>(parsedState);

        // 检查容器类型是否正确
        if (b->Type != "containerType") {  // 假设containerType是 "containerType"
            throw myerror("容器 " + container + " 不是有效的容器类型 (它是 " + b->Type + ")");
        }

        // 获取网络接口
        //如果在容器构建的过程中需要网络支持（如连接外部资源或设置虚拟网络），这函数是必要的
        // b->NetworkInterface = getNetworkInterface(store, b->CNIConfigDir, b->CNIPluginPath);

        // 设置 store
        b->store = store;

        // 修正配置和设置日志
        fixupConfig(b, nullptr);
        // setupLogger(b);

        // 如果CommonBuildOpts为空，初始化它
        if (!b->CommonBuildOpts) {
            b->CommonBuildOpts = std::make_shared<CommonBuildOptions>();
        }

    } catch (const myerror& e) {
        // 处理 myerror 类型的错误
        std::cerr << "错误: " << e.what() << std::endl;
        throw;

    } catch (const boost::json::error& e) {
        // 处理 JSON 解析错误
        throw myerror("解析JSON文件时发生错误: " + std::string(e.what()));

    } catch (const boost::filesystem::filesystem_error& e) {
        // 处理文件系统错误
        throw myerror("文件系统操作失败: " + std::string(e.what()));

    }
    return b;
}



