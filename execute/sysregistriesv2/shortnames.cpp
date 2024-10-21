#include "sysregistriesv2/shortnames.h"
#include "reference/reference.h"
#include "cobra/error.h"
#include "lockfile/lockfile.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
std::tuple<std::shared_ptr<Named_interface>,std::string> ResolveShortNameAlias(std::shared_ptr<SystemContext> ctx,std::string name){
    try{
        validateShortName(name);
    }catch(const myerror& e){
        throw;
    }
    auto confPath="D:\\oci_images\\short-name-aliases.conf";
    
    return std::make_tuple(nullptr,"");

}
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
        config->Aliases[pt.get<std::string>("Aliases.Alias1")] = pt.get<std::string>("Aliases.Alias2");
    }
    return std::make_tuple(config,nullptr);
}
// validateShortName 函数：解析别名并检查它是否为短名且不包含标签或摘要
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