#include "image/libimage/normalize.h"
#include "image/types/reference/normalize.h"
#include "utils/common/error.h"
#include "image/image_types/v1/descriptor.h"
#include "filesys/platforms/platforms.h"
#include "filesys/platforms/default_unix.h"
std::shared_ptr<Named_interface> NormalizeName(std::string name){
    auto ref=Parse(name);
    auto named=std::dynamic_pointer_cast<Named_interface>(ref);
    if(named==nullptr){
        return nullptr;
    }
    auto registry=Domain(named);
    name="localhost/"+name;
    named=ParseNormalizedNamed(name);

    return named;
}
std::shared_ptr<Named_interface>normalizeTaggedDigestedNamed(std::shared_ptr<Named_interface> named);
std::tuple<std::string, std::shared_ptr<Named_interface>> normalizeTaggedDigestedString(std::string s) {
    std::shared_ptr<Reference_interface> ref;
    try{
        ref=Parse(s);
    }catch(const myerror& e){
        throw;
    }
    auto named=std::dynamic_pointer_cast<Named_interface>(ref);
    try{
        named=normalizeTaggedDigestedNamed(named);
    }catch(const myerror& e){
        throw;
    }
    return {named->String(),named};
    // return std::make_tuple("", nullptr);
}

std::shared_ptr<Named_interface>normalizeTaggedDigestedNamed(std::shared_ptr<Named_interface> named){

    return named;
}
namespace LibImage {
    std::tuple<std::string,std::string,std::string> Normalize(std::string rawOS,std::string rawArch,std::string rawVariant) {
        auto platformSpec=std::make_shared<Platform>();
        platformSpec->OS=rawOS;
        platformSpec->Architecture=rawArch;
        platformSpec->Variant=rawVariant;
        auto normalizedSpec=::Normalize(platformSpec);
        if(normalizedSpec->Variant==""&&rawVariant!=""){
            normalizedSpec->Variant=rawVariant;
        }
        auto rawPlatform=ToString(normalizedSpec->OS,normalizedSpec->Architecture,normalizedSpec->Variant);
        std::shared_ptr<Platform> normalizedPlatform;
        try{
            normalizedPlatform=PlatForms::Parse(rawPlatform);
        }catch(const myerror& e){
            throw;
        }
        std::string os,arch,variant;
        os=rawOS;
        if(rawOS!=""){
            os=normalizedPlatform->OS;
        }
        arch=rawArch;
        if(rawArch!=""){
            arch=normalizedPlatform->Architecture;
        }
        variant=rawVariant;
        if(rawVariant!=""||(rawVariant==""&&normalizedPlatform->Variant!="")){
            variant=normalizedPlatform->Variant;
        }
        return std::make_tuple(os,arch,variant);
        // return std::make_tuple(rawOS,rawArch,rawVariant);
        
    }
}

// 将 os, arch, variant 转换为字符串
std::string ToString(const std::string& os, const std::string& arch, const std::string& variant) {
    std::string finalOS = os.empty() ? GOOS : os;
    std::string finalArch = arch.empty() ? GOARCH : arch;
    
    std::ostringstream result;
    if (variant.empty()) {
        result << finalOS << "/" << finalArch;
    } else {
        result << finalOS << "/" << finalArch << "/" << variant;
    }
    
    return result.str();
}