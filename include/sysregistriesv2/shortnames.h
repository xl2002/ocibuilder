#if !defined(SYSREgistriesV2_SHORTNAMES_H)
#define SYSREgistriesV2_SHORTNAMES_H
#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <map>
#include "reference/reference.h"
#include "types/types.h"
#include "digest/digest.h"
#include "define/types.h"
struct shortNameAliasConf{
    std::map<std::string,std::string> Aliases;
};
struct alias{
    std::shared_ptr<Named_interface> value;
    std::string configOrigin;
    alias()=default;
    alias(std::shared_ptr<Named_interface> value,std::string configOrigin):value(value),configOrigin(configOrigin){};
};
struct shortNameAliasCache{
    std::map<std::string,std::shared_ptr<alias>> namedAliases;
};

std::tuple<std::shared_ptr<Named_interface>,std::string> ResolveShortNameAlias(std::shared_ptr<SystemContext> ctx,std::string name);
void validateShortName(const std::string& name);
void traverse_ptree_to_map(const boost::property_tree::ptree& pt, std::map<std::string, std::string>& result_map, const std::string& path = "");
std::tuple<std::shared_ptr<shortNameAliasConf>,std::shared_ptr<shortNameAliasCache>> loadShortNameAliasConf(std::string confPath);
std::shared_ptr<Named_interface> parseShortNameValue(const std::string& name);
std::shared_ptr<shortNameAliasCache> newShortNameAliasCache(std::string path,std::shared_ptr<shortNameAliasConf> conf);
#endif // SYSREgistriesV2_SHORTNAMES_H)
