/**
 * @file shortnames.cpp
 * @brief 实现短名称别名解析相关功能
 */
#include "image/shortnames/sysregistriesv2/shortnames.h"
#include "image/types/reference/reference.h"
#include "utils/common/error.h"
#include "image/types/reference/normalize.h"
#include "storage/lockfile/lockfile.h"
#include "image/shortnames/sysregistriesv2/system_registries_v2.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
/**
 * @brief 解析短名称别名
 * @param ctx 系统上下文，可为nullptr
 * @param name 要解析的短名称
 * @return 返回tuple包含:
 *   - shared_ptr<Named_interface>: 解析后的命名接口指针(如果找到别名)
 *   - string: 配置来源路径
 * @throws myerror 当名称验证失败或配置加载失败时抛出
 * @details 
 * 1. 首先验证输入名称格式
 * 2. 加载短名称别名配置文件
 * 3. 先在本地缓存中查找别名
 * 4. 然后在系统配置中查找别名
 */
std::tuple<std::shared_ptr<Named_interface>,std::string> ResolveShortNameAlias(std::shared_ptr<SystemContext> ctx,std::string name){
    try{
        validateShortName(name);
    }catch(const myerror& e){
        throw;
    }
    auto confPath=".\\oci_images\\short-name-aliases.conf";
    std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    std::shared_ptr<shortNameAliasCache> aliasCache;
    try{
        std::tie(std::ignore,aliasCache)=loadShortNameAliasConf(confPath);
    }catch(const myerror& e){
        throw;
    }
    auto it=aliasCache->namedAliases.find(name);
    if(it!=aliasCache->namedAliases.end()){
        return std::make_tuple(it->second->value,it->second->configOrigin);
    }
    auto config=getConfig(ctx);
    auto it2=config->aliasCache->namedAliases.find(name);
    if(it2!=config->aliasCache->namedAliases.end()){
        return std::make_tuple(it2->second->value,it2->second->configOrigin);
    }
    return std::make_tuple(nullptr,"");

}
// 遍历ptree并将数据存储到map
// void traverse_ptree_to_map(const boost::property_tree::ptree& pt, std::map<std::string, std::string>& result_map, const std::string& path) {
//     for (const auto& kv : pt) {
//         std::string full_path = path.empty() ? kv.first : path + "." + kv.first;

//         if (kv.second.empty()) {
//             // 如果没有子节点，将 key 和 value 存入 map
//             result_map[full_path] = kv.second.get_value<std::string>();
//         } else {
//             // 如果有子节点，递归遍历
//             traverse_ptree_to_map(kv.second, result_map, full_path);
//         }
//     }
// }
/**
 * @brief 加载短名称别名配置文件
 * @param confPath 配置文件路径
 * @return 返回tuple包含:
 *   - shared_ptr<shortNameAliasConf>: 配置对象
 *   - shared_ptr<shortNameAliasCache>: 缓存对象
 * @throws myerror 当配置文件解析失败时抛出
 * @details 
 * 1. 使用boost.property_tree解析INI格式配置文件
 * 2. 创建并初始化别名缓存
 */
std::tuple<std::shared_ptr<shortNameAliasConf>,std::shared_ptr<shortNameAliasCache>> loadShortNameAliasConf(std::string confPath){
    auto config=std::make_shared<shortNameAliasConf>();
    boost::property_tree::ptree pt;
    // 尝试读取文件
    try {
        boost::property_tree::ini_parser::read_ini(confPath, pt);
    } catch (const boost::property_tree::ini_parser_error& e) {
        throw myerror("无法解析配置文件: " + confPath + ": " + e.what());
    }
    if(!pt.empty()){
        // traverse_ptree_to_map(pt,config->Aliases);
    }
    try{
        auto cache=newShortNameAliasCache(confPath,config);
        return std::make_tuple(config,cache);
    }catch(const myerror& e){
        return std::make_tuple(nullptr,nullptr);
    }
}
// validateShortName 函数：解析别名并检查它是否为短名且不包含标签或摘要
/**
 * @brief 验证短名称格式
 * @param name 要验证的名称
 * @throws myerror 当名称不符合短名称规范时抛出
 * @details 
 * 检查名称:
 * 1. 必须是可解析的引用
 * 2. 不能包含digest
 * 3. 不能包含tag
 * 4. 必须是命名引用
 * 5. 不能包含registry信息(.:或localhost)
 */
void validateShortName(const std::string& name) {
    // 解析名称
    std::shared_ptr<Reference_interface> repo;
    try {
        repo = Parse(name);
    } catch (const myerror& e) {
        throw myerror("cannot parse short name: \"" + name + "\": " + e.what());
    }

    // 检查是否包含摘要
    if (std::dynamic_pointer_cast<Digested_interface>(repo)) {
        throw myerror("invalid short name \"" + name + "\": must not contain digest");
    }

    // 检查是否包含标签
    if (std::dynamic_pointer_cast<Tagged_interface>(repo)) {
        throw myerror("invalid short name \"" + name + "\": must not contain tag");
    }

    // 检查是否有名字
    auto  named = std::dynamic_pointer_cast<Named_interface>(repo);
    if (!named) {
        throw myerror("invalid short name \"" + name + "\": no name");
    }

    // 获取域名并检查是否包含非法字符
    std::string registry = Domain(named);
    if (registry.find_first_of(".:") != std::string::npos || registry == "localhost") {
        throw myerror("invalid short name \"" + name + "\": must not contain registry");
    }
}
// validateShortName 函数：解析别名并检查它是否为短名且不包含标签或摘要
/**
 * @brief 解析短名称值
 * @param name 要解析的名称
 * @return 返回规范化后的命名接口
 * @throws myerror 当名称不符合短名称规范时抛出
 * @details 
 * 检查名称:
 * 1. 必须是可解析的引用
 * 2. 不能包含digest
 * 3. 不能包含tag
 * 4. 必须是命名引用
 * 5. 不能包含registry信息(.:或localhost)
 * 6. 最后返回规范化后的名称
 */
std::shared_ptr<Named_interface> parseShortNameValue(const std::string& name) {
    // 解析名称
    std::shared_ptr<Reference_interface> repo;
    try {
        repo = Parse(name);
    } catch (const myerror& e) {
        throw myerror("parsing alias: \"" + name + "\": " + e.what());
    }

    // 检查是否包含摘要
    if (std::dynamic_pointer_cast<Digested_interface>(repo)) {
        throw myerror("invalid alias \"" + name + "\": must not contain digest");
    }

    // 检查是否包含标签
    if (std::dynamic_pointer_cast<Tagged_interface>(repo)) {
        throw myerror("invalid alias \"" + name + "\": must not contain tag");
    }

    // 检查是否有名字
    auto  named = std::dynamic_pointer_cast<Named_interface>(repo);
    if (!named) {
        throw myerror("invalid alias \"" + name + "\": no name");
    }

    // 获取域名并检查是否包含非法字符
    std::string registry = Domain(named);
    if (registry.find_first_of(".:") != std::string::npos || registry == "localhost") {
        throw myerror("invalid short name \"" + name + "\": must not contain registry");
    }
    named=ParseNormalizedNamed(name);
    return named;
}
/**
 * @brief 创建新的短名称别名缓存
 * @param path 配置文件路径
 * @param conf 配置对象
 * @return 返回初始化好的缓存对象
 * @details 
 * 1. 遍历配置中的所有别名
 * 2. 验证每个别名键和值
 * 3. 将有效别名存入缓存
 */
std::shared_ptr<shortNameAliasCache> newShortNameAliasCache(std::string path,std::shared_ptr<shortNameAliasConf> conf){
    auto res=std::make_shared<shortNameAliasCache>();
    for(auto kv:conf->Aliases){
        validateShortName(kv.first);
        if(kv.second.empty()){
            
        }
        auto named=parseShortNameValue(kv.second);
        res->namedAliases[kv.first]=std::make_shared<alias>(named,kv.second);

    }
    return res;
}
