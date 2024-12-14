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
void setRegistriesConfPath(std::shared_ptr<SystemContext> systemContext) {
    if(systemContext->SystemRegistriesConfDirPath!="") {
        return;
    }
    return;
}

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

std::vector<std::string> Runtime::copyFromRegistry(std::shared_ptr<ImageReference_interface> ref,std::string inputName,std::shared_ptr<PullPolicy> pullPolicy,std::shared_ptr<PullOptions> options) {

    if(!pullPolicy->Validate()) {
        return {};
    }
    if(!options->AllTags) {
        return this->copySingleImageFromRegistry(inputName,pullPolicy,options);
    }
    return {};
}

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
