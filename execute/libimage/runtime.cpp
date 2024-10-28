#include "libimage/runtime.h"
#include "logrus/exported.h"
#include "config/new.h"
#include "alltransports/alltransports.h"
#include "docker/docker_transport.h"
#include "libimage/normalize.h"
#include "go/string.h"
#include "shortnames/shortnames.h"
#include "reference/regexp.h"
#include "storage/images.h"
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

std::vector<std::shared_ptr<libimage::Image>> Runtime::Pull(std::string name,std::shared_ptr<PullPolicy> pullPolicy,std::shared_ptr<PullOptions> options) {
    // Debugf("Pulling image %s (policy: %s)",name.c_str(),pullPolicy->String().c_str());
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
    
    return {};
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
    std::shared_ptr<libimage::Image> localImage;
    std::string resolvedImageName;
    auto lookupImageOptions=std::make_shared<LookupImageOptions>();
    lookupImageOptions->Variant=options->Variant;
    if(options->Architecture!="amd64"){
        lookupImageOptions->Architecture=options->Architecture;
    }
    if(options->OS!="linux"){
        lookupImageOptions->OS=options->OS;
    }
    // std::shared_ptr<libimage::Image> localImage;
    // std::string resolvedImageName;
    try{
        std::tie(localImage,resolvedImageName)=this->LookupImage(imageName,lookupImageOptions);
    }catch(const myerror& e) {
        throw;
    }
    return {};

}

std::tuple<std::shared_ptr<libimage::Image>,std::string> Runtime::LookupImage(std::string name,std::shared_ptr<LookupImageOptions> options) {
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
    std::tie(options->OS,options->Architecture,options->Variant)=libimage::Normalize(options->OS,options->Architecture,options->Variant);
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
    return{};

}

std::shared_ptr<libimage::Image> Runtime::lookupImageInLocalStorage(std::string name,std::string candidate,std::shared_ptr<Named_interface> namedCandidate,std::shared_ptr<LookupImageOptions> options) {
    std::shared_ptr<storage::Image> img;
    std::shared_ptr<ImageReference_interface> ref;
    if(namedCandidate!=nullptr) {
        namedCandidate=TagNameOnly(namedCandidate);
        ref=Transport->NewStoreReference(this->store,namedCandidate,"");
    }
    std::tie(std::ignore,img)=ResolveReference(ref);
    
    return nullptr;
}