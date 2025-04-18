#include "image/libimage/runtime.h"
#include "utils/logger/logrus/exported.h"
#include "config/new.h"
#include "image/transports/alltransports.h"
#include "image/image_types/docker/docker_transport.h"
#include "image/libimage/normalize.h"
#include "utils/common/go/string.h"
#include "image/shortnames/shortnames.h"
#include "image/types/reference/regexp.h"
#include "storage/storage/images.h"
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
/**
 * @brief 从存储创建运行时实例
 * 
 * @param store 存储接口指针
 * @param options 运行时选项指针(可选)
 * @return std::shared_ptr<Runtime> 返回创建的Runtime实例
 * 
 * @details 该函数:
 * 1. 如果options为空则创建默认选项
 * 2. 设置系统上下文
 * 3. 初始化注册表配置路径
 * 4. 创建并返回Runtime实例
 * 
 * @note 如果SystemContext未设置会创建默认上下文
 */
std::shared_ptr<Runtime> RuntimeFromStore(std::shared_ptr<Store> store,std::shared_ptr<RuntimeOptions> options) {
    if( options==nullptr ) {
        options=std::make_shared<RuntimeOptions>();
    }
    auto systemContext=std::shared_ptr<SystemContext>();
    if(options->SystemContext!=nullptr) {
        systemContext=options->SystemContext;
    }else{
        systemContext=std::make_shared<SystemContext>();
    }
    if(systemContext->BigFilesTemporaryDir=="") {
        
    }
    setRegistriesConfPath(systemContext);
    auto ret_runtime=std::make_shared<Runtime>();
    ret_runtime->store=store;
    ret_runtime->systemContext=systemContext;
    return ret_runtime;
}
/**
 * @brief 设置注册表配置文件路径
 * 
 * @param systemContext 系统上下文指针
 * 
 * @details 如果systemContext中未设置SystemRegistriesConfDirPath，
 * 则设置默认路径
 */
void setRegistriesConfPath(std::shared_ptr<SystemContext> systemContext) {
    if(systemContext->SystemRegistriesConfDirPath!="") {
        return;
    }
    return;
}

/**
 * @brief 拉取镜像
 * 
 * @param name 镜像名称或路径
 * @param pullPolicy 拉取策略
 * @param options 拉取选项(可选)
 * @return std::vector<std::shared_ptr<LibImage::Image>> 返回拉取的镜像列表
 * 
 * @details 该函数:
 * 1. 检查是否为本地路径或scratch镜像
 * 2. 解析镜像名称
 * 3. 根据传输类型调用不同拉取逻辑
 * 4. 检查平台匹配性
 * 
 * @throw myerror 如果镜像名称解析失败或拉取失败
 * @see copyFromRegistry
 */
std::vector<std::shared_ptr<LibImage::Image>> Runtime::Pull(std::string name,std::shared_ptr<PullPolicy> pullPolicy,std::shared_ptr<PullOptions> options) {
    // Debugf("Pulling image %s (policy: %s)",name.c_str(),pullPolicy->String().c_str());
    if(fs::exists(name)) {//from 后面为文件夹
        auto localImages=std::vector<std::shared_ptr<LibImage::Image>>{};
        std::shared_ptr<LibImage::Image> fromimage=std::make_shared<LibImage::Image>();
        //在overlay创建layer层，并模拟出Image
        auto store=this->store;
        auto rlstore=store->bothLayerStoreKinds();
        auto layerOptions=std::make_shared<LayerOptions>();
        auto diff=std::ifstream{};
        std::string id="";
        std::shared_ptr<Layer> clayer;
        std::tie(clayer,std::ignore)=rlstore->create(id,nullptr,{},"",{},layerOptions,true,diff);
        fromimage->storageImage->ID=clayer->ID;
        localImages.push_back(fromimage);
        return localImages;
    }else if(name=="scratch") {//基础镜像为空
        auto localImages=std::vector<std::shared_ptr<LibImage::Image>>{};
        std::shared_ptr<LibImage::Image> fromimage=std::make_shared<LibImage::Image>();
        localImages.push_back(fromimage);
        return localImages;
    }else{
        std::cerr<<"Dockerfile FROM path not found: "<<name<<std::endl;
    }
    if(!this->eventChannel.empty()) {
        
    }
    if(options==nullptr) {
        options=std::make_shared<PullOptions>();
    }
    auto defaultConfig=Config_defaut();
    if(options->MaxRetries==nullptr) {
        options->MaxRetries=std::make_shared<uint32_t>(defaultConfig->Engine->Retry);
    }
    if(options->RetryDelay<std::chrono::milliseconds::zero()) {

    }
    std::string possiblyUnqualifiedName;
    std::shared_ptr<ImageReference_interface> ref;
    try{
        ref=ParseImageName(name);
    }catch(const myerror& e) {
        auto t=TransportFromImageName(name);
        if(t!=nullptr&& t->Name()!=docker_Transport->Name()) {
            throw;
        }
        std::string normalizedName;
        try{
            std::tie(normalizedName,std::ignore)=normalizeTaggedDigestedString(name);
        }catch(const myerror& e) {
            throw;
        }
        name=normalizedName;
        std::shared_ptr<ImageReference_interface> dockerRef;
        try{
            dockerRef=ParseImageName("docker://"+name);
        }catch(const myerror& e) {
            throw;
        }
        ref=dockerRef;
        possiblyUnqualifiedName=name;
    }
    if(ref->Transport()->Name()==docker_Transport->Name()) {
        
    }
    if(options->AllTags && ref->Transport()->Name()!=docker_Transport->Name()) {
        return {};
    }
    if(options->Architecture=="") {
        options->Architecture=this->systemContext->ArchitectureChoice;
    }
    if(options->OS=="") {
        options->OS=this->systemContext->OSChoice;
    }
    if(options->Variant=="") {
        options->Variant=this->systemContext->VariantChoice;
    }
    std::vector<std::string> pulledImages;
    auto namestr=ref->Transport()->Name();
    try{
        if(namestr==docker_Transport->Name()) {
            pulledImages=this->copyFromRegistry(ref,possiblyUnqualifiedName,pullPolicy,options);
        }
    }catch(const myerror& e) {
        throw;
    }
    auto localImages=std::vector<std::shared_ptr<LibImage::Image>>{};
    for(auto iname:pulledImages) {
        try{
            std::shared_ptr<LibImage::Image> image;
            std::tie(image,std::ignore)=this->LookupImage(iname,nullptr);
            auto matched=image->matchesPlatform(options->OS,options->Architecture,options->Variant);
            if(!matched) {
                return {};
            }
            localImages.push_back(image);
        }catch(const myerror& e) {
            throw;
        }
    }
    return localImages;
}

/**
 * @brief 从注册表复制镜像
 * 
 * @param ref 镜像引用
 * @param inputName 输入镜像名称
 * @param pullPolicy 拉取策略 
 * @param options 拉取选项
 * @return std::vector<std::string> 返回拉取的镜像名称列表
 * 
 * @details 根据pullPolicy和options决定是拉取单个镜像还是所有标签
 * 
 * @see copySingleImageFromRegistry
 */
std::vector<std::string> Runtime::copyFromRegistry(std::shared_ptr<ImageReference_interface> ref,std::string inputName,std::shared_ptr<PullPolicy> pullPolicy,std::shared_ptr<PullOptions> options) {

    if(!pullPolicy->Validate()) {
        return {};
    }
    if(!options->AllTags) {
        return this->copySingleImageFromRegistry(inputName,pullPolicy,options);
    }
    return {};
}

/**
 * @brief 从注册表复制单个镜像
 * 
 * @param imageName 镜像名称
 * @param pullPolicy 拉取策略
 * @param options 拉取选项
 * @return std::vector<std::string> 返回拉取的镜像名称
 * 
 * @details 该函数:
 * 1. 验证拉取策略
 * 2. 查找本地镜像
 * 3. 检查镜像是否损坏
 * 
 * @throw myerror 如果查找镜像失败
 */
std::vector<std::string> Runtime::copySingleImageFromRegistry(std::string imageName,std::shared_ptr<PullPolicy> pullPolicy,std::shared_ptr<PullOptions> options) {
    if(!pullPolicy->Validate()) {
        return {};
    }
    std::shared_ptr<LibImage::Image> localImage;
    std::string resolvedImageName;
    auto lookupImageOptions=std::make_shared<LookupImageOptions>();
    lookupImageOptions->Variant=options->Variant;
    if(options->Architecture!="amd64"){
        lookupImageOptions->Architecture=options->Architecture;
    }
    if(options->OS!="linux"){
        lookupImageOptions->OS=options->OS;
    }
    // std::shared_ptr<LibImage::Image> localImage;
    // std::string resolvedImageName;
    try{
        std::tie(localImage,resolvedImageName)=this->LookupImage(imageName,lookupImageOptions);
    }catch(const myerror& e) {
        throw;
    }
    if(localImage!=nullptr) {
        if(localImage->isCorrupted(imageName)) {
            localImage=nullptr;
        }
        return {resolvedImageName};
    }
    return {""};

}

/**
 * @brief 查找镜像
 * 
 * @param name 镜像名称
 * @param options 查找选项(可选)
 * @return std::tuple<std::shared_ptr<LibImage::Image>,std::string> 返回找到的镜像及其名称
 * 
 * @details 该函数:
 * 1. 解析镜像名称
 * 2. 规范化平台选项
 * 3. 解析本地候选镜像
 * 4. 在本地存储中查找匹配的镜像
 * 
 * @throw myerror 如果名称解析失败或查找失败
 * @see lookupImageInLocalStorage
 */
std::tuple<std::shared_ptr<LibImage::Image>,std::string> Runtime::LookupImage(std::string name,std::shared_ptr<LookupImageOptions> options) {
    // Debugf("Looking up image %s in local storage",name.c_str());
    if(options==nullptr) {
        options=std::make_shared<LookupImageOptions>();
    }
    std::shared_ptr<ImageReference_interface> ref;
    ref=ParseImageName(name);
    std::string  normalizedName;
    std::shared_ptr<Named_interface> possiblyUnqualifiedNamedReference;
    std::tie(normalizedName,possiblyUnqualifiedNamedReference)=normalizeTaggedDigestedString(name);
    name=normalizedName;

    bool byDigest=false;
    auto originalName=name;
    if(hasPrefix(name,"sha256:")) {
        byDigest=true;
        name=TrimPrefix(name,"sha256:");
    }
    auto byFullID=anchoredIdentifierRegexp->Match(name);
    if(byFullID&& !byFullID) {

    }
    if(byFullID||byDigest){

    }
    if(options->Architecture==""){
        
    }
    if(options->OS=="") {
        options->OS=this->systemContext->OSChoice;
    }
    if(options->Variant=="") {
        options->Variant=this->systemContext->VariantChoice;
    }
    std::tie(options->OS,options->Architecture,options->Variant)=LibImage::Normalize(options->OS,options->Architecture,options->Variant);
    std::vector<std::shared_ptr<Named_interface>> candidates;
    try{
        candidates=ResolveLocally(this->systemContext,name);
    }catch(const myerror& e) {
        throw;
    }
    // ParseDockerRef(name);
    // for(auto candidate:candidates) {
    //     this->lookupImageInLocalStorage(name,candidate->String(),candidate,options);
    // }
    ParseDockerRef(name);
    // 遍历 candidates
    for (const auto& candidate : candidates) {
        // 尝试在本地存储中查找图像
        std::shared_ptr<LibImage::Image> img;
        try {
            // 调用 lookupImageInLocalStorage，传入相应的参数
            img = this->lookupImageInLocalStorage(name, candidate->String(), candidate, options);
        } catch (const myerror& err) {
            // 如果发生错误，返回空图像指针、空字符串，并抛出异常
            throw;
        }

        // 如果找到了图像，则返回图像和候选项的字符串表示
        if (img != nullptr) {
            return std::make_tuple(img, candidate->String());
        }
    }
    return{nullptr,""};

}

// std::shared_ptr<LibImage::Image> Runtime::lookupImageInLocalStorage(std::string name,std::string candidate,std::shared_ptr<Named_interface> namedCandidate,std::shared_ptr<LookupImageOptions> options) {
/**
 * @brief 在本地存储中查找镜像
 * 
 * @param name 镜像名称
 * @param candidate 候选名称
 * @param namedCandidate 命名的候选引用
 * @param options 查找选项
 * @return std::shared_ptr<LibImage::Image> 返回找到的镜像
 * 
 * @details 该函数:
 * 1. 解析存储引用
 * 2. 检查平台匹配性
 * 3. 将存储镜像转换为LibImage格式
 * 
 * @throw myerror 如果引用解析失败或平台不匹配
 * @see storageToImage
 */
std::shared_ptr<LibImage::Image> Runtime::lookupImageInLocalStorage(std::string name,std::string candidate,std::shared_ptr<Named_interface> namedCandidate,std::shared_ptr<LookupImageOptions> options){
    std::shared_ptr<storage::Image> img;
    std::shared_ptr<ImageReference_interface> ref;
    if(namedCandidate!=nullptr) {
        namedCandidate=TagNameOnly(namedCandidate);
        try{
            ref=Transport->NewStoreReference(this->store,namedCandidate,"");
            std::tie(std::ignore,img)=ResolveReference(ref);
        }catch(const myerror& e) {
            throw;
        }
    }
    ref=Transport->ParseStoreReference(this->store,img->ID);
    if(ref==nullptr) {
        throw myerror("failed to parse image ID: "+img->ID);
    }
    auto image=this->storageToImage(img,ref);
    if(!image->matchesPlatform(options->OS,options->Architecture,options->Variant)) {
        throw myerror("image does not match platform: "+options->OS+"/"+options->Architecture+"/"+options->Variant);
    }

    // if(img!=nullptr) {
    //     return img;
    // }
    
    return image;
}
/**
 * @brief 将存储镜像转换为LibImage格式
 * 
 * @param img 存储镜像
 * @param ref 镜像引用
 * @return std::shared_ptr<LibImage::Image> 返回转换后的镜像
 * 
 * @details 创建新的LibImage实例并设置相关属性
 */
std::shared_ptr<LibImage::Image> Runtime::storageToImage(std::shared_ptr<storage::Image> img,std::shared_ptr<ImageReference_interface> ref) {
    auto image=std::make_shared<LibImage::Image>();
    image->runtime=std::make_shared<Runtime>(*this);
    image->storageImage=img;
    image->storageReference=ref;
    return image;
}
/**
 * @brief 从source推送镜像到destination
 * 
 * @param source 
 * @param destination 
 * @param options 
 * @return std::vector<uint8_t> 
 */
std::vector<uint8_t> Runtime::Push(std::string source,std::string destination,std::shared_ptr<PushOptions> options){
    // 1. 从镜像库里读取镜像
    // image, resolvedSource, err := r.LookupImage(source, nil)

    // 2. 获得镜像相关的信息
    // srcRef, err := image.StorageReference()

    // 3. 分析目标位置信息
    // destRef, err := alltransports.ParseImageName(destination)

    // 4. 构建dockerclient和dest准备上传镜像

    // 5. 获得目标仓库的令牌token

    // 6. 获得镜像所有层的ID，循环每一层，读取每一层的数据，传输到远程库相应的位置
    // 传输逻辑：首先判断远程端是否有同名的镜像层，如果有，就跳过，如果没有，就上传
    return std::vector<uint8_t>();
}
