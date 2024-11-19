#include "libimage/normalize.h"
#include "cobra/error.h"
#include "v1/descriptor.h"
#include "platforms/platforms.h"
#include "platforms/default_unix.h"
std::shared_ptr<named> NormalizeName(std::string name){
    
    return nullptr;
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
namespace libimage {
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
            normalizedPlatform=platforms::Parse(rawPlatform);
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