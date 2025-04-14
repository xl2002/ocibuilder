/**
 * @file shortnames.cpp
 * @brief 实现短名称解析相关功能
 */
#include "image/shortnames/shortnames.h"
#include "utils/common/error.h"
#include "image/shortnames/sysregistriesv2/shortnames.h"
#include "image/shortnames/sysregistriesv2/system_registries_v2.h"
/**
 * @brief 本地解析短名称引用
 * @param ctx 系统上下文，可为nullptr
 * @param name 要解析的名称(可以是短名称或完整名称)
 * @return 返回可能的候选引用列表
 * @throws myerror 当名称解析失败时抛出
 * @details 
 * 1. 首先解析输入名称是否为短名称
 * 2. 检查系统上下文配置
 * 3. 处理别名解析
 * 4. 从非限定搜索注册表中生成候选列表
 */
std::vector<std::shared_ptr<Named_interface>> ResolveLocally(std::shared_ptr<SystemContext> ctx,std::string name){
    bool isShort;
    std::shared_ptr<Named_interface> shortRef;
    std::tie(isShort,shortRef)=parseUnnormalizedShortName(name);
    if(!isShort){

    }
    std::vector<std::shared_ptr<Named_interface>> candidates;
    auto completeCandidates=[&](std::vector<std::string> registries){
        for(auto r:registries){
            std::shared_ptr<Named_interface> named=ParseNormalizedNamed(r+"/"+name);
            named=TagNameOnly(named);
            candidates.push_back(named);
        }
        return candidates;
    };
    if(ctx!=nullptr&& ctx->PodmanOnlyShortNamesIgnoreRegistriesConfAndForceDockerHub){
        
    }
    bool isTagged, isDigested;
    std::shared_ptr<Named_interface> shortNameRepo;
    std::string tag;
    std::shared_ptr<Digest> digest;
    std::tie(isTagged,isDigested,shortNameRepo,tag,digest)=splitUserInput(shortRef);
    std::shared_ptr<Named_interface> nameAlias;
    std::tie(nameAlias,std::ignore)=ResolveShortNameAlias(ctx,shortNameRepo->String());
    if(nameAlias!=nullptr){
        if(isTagged){
            nameAlias=WithTag(nameAlias,tag);
        }
        nameAlias=TagNameOnly(nameAlias);
        candidates.push_back(nameAlias);
    }
    auto unqualifiedSearchRegistries=UnqualifiedSearchRegistries(ctx);
    std::vector<string> strvec;
    for(auto m:unqualifiedSearchRegistries){
        strvec.push_back(m.second);
    }
    strvec.push_back("localhost");
    return completeCandidates(strvec);
}

/**
 * @brief 解析未规范化的短名称
 * @param input 输入的名称字符串
 * @return 返回tuple包含:
 *   - bool: 是否为短名称
 *   - shared_ptr<Named_interface>: 解析后的命名接口指针(如果解析成功)
 * @throws myerror 当名称格式无效时抛出
 */
std::tuple<bool,std::shared_ptr<Named_interface>> parseUnnormalizedShortName(std::string input) {
    std::shared_ptr<Reference_interface> ref;
    try{
        ref=Parse(input);
    }catch(const myerror& e){
        return {false,nullptr};
    }
    auto named=std::dynamic_pointer_cast<Named_interface>(ref);
    if(named==nullptr){
        return {false,nullptr};
    }
    auto r=Domain(named);
    return {true,named};
}

/**
 * @brief 分割用户输入的名称引用
 * @param named 输入的命名接口指针
 * @return 返回tuple包含:
 *   - bool: 是否包含tag
 *   - bool: 是否包含digest
 *   - shared_ptr<Named_interface>: 规范化后的命名接口
 *   - string: tag值(如果有)
 *   - shared_ptr<Digest>: digest值(如果有)
 * @throws myerror 当输入不是tagged引用时抛出
 */
std::tuple<bool,bool,std::shared_ptr<Named_interface>,std::string,std::shared_ptr<Digest>> splitUserInput(std::shared_ptr<Named_interface> named) {
    bool isTagged, isDigested;
    std::shared_ptr<Named_interface> normalized;
    std::string tag;
    std::shared_ptr<Digest> digest;
    
    auto tagged=std::dynamic_pointer_cast<NamedTagged_interface>(named);
    if(tagged==nullptr){
        throw myerror("not a tagged reference");
    }
    isTagged=true;
    tag=tagged->Tag();
    auto digested=std::dynamic_pointer_cast<Digested_interface>(named);
    if(digested!=nullptr){
        isDigested=true;
        digest=std::make_shared<Digest>(digested->Digest());
    }
    normalized=TrimNamed(named);
    return {isTagged,isDigested,normalized,tag,digest};
}
