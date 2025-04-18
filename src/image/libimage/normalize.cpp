#include "image/libimage/normalize.h"
#include "image/types/reference/normalize.h"
#include "utils/common/error.h"
#include "image/image_types/v1/descriptor.h"
#include "filesys/platforms/platforms.h"
#include "filesys/platforms/default_unix.h"
/**
 * @brief 标准化镜像名称
 * @param name 原始镜像名称
 * @return std::shared_ptr<Named_interface> 标准化后的镜像名称接口指针
 */
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
/**
 * @brief 标准化带标签和摘要的镜像名称
 * @param named 原始镜像名称接口指针
 * @return std::shared_ptr<Named_interface> 标准化后的镜像名称接口指针
 */
std::shared_ptr<Named_interface>normalizeTaggedDigestedNamed(std::shared_ptr<Named_interface> named);
/**
 * @brief 标准化带标签和摘要的镜像名称字符串
 * @param s 原始镜像名称字符串
 * @return std::tuple<std::string, std::shared_ptr<Named_interface>> 返回标准化后的名称字符串和接口指针
 * @throws myerror 如果解析失败
 */
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
    /**
     * @brief 标准化平台信息(OS/Arch/Variant)
     * @param rawOS 原始操作系统
     * @param rawArch 原始架构
     * @param rawVariant 原始变体
     * @return std::tuple<std::string,std::string,std::string> 返回标准化后的OS/Arch/Variant
     * @throws myerror 如果解析失败
     */
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
/**
 * @brief 将平台信息转换为字符串
 * @param os 操作系统
 * @param arch 架构
 * @param variant 变体
 * @return std::string 返回格式为"os/arch"或"os/arch/variant"的字符串
 */
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
