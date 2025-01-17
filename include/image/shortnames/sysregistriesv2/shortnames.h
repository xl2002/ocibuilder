#if !defined(IMAGE_SHORTNAMES_SYSREgistriesV2_SHORTNAMES_H)
#define IMAGE_SHORTNAMES_SYSREgistriesV2_SHORTNAMES_H
#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <map>
#include "image/types/reference/reference.h"
#include "image/types/types.h"
#include "image/digest/digest.h"
#include "image/types/define/types.h"
class shortNameAliasConf{
    public:
    std::map<std::string,std::string> Aliases;
};
class alias{
    public:
    std::shared_ptr<Named_interface> value=nullptr;
    std::string configOrigin;
    alias()=default;
    alias(std::shared_ptr<Named_interface> value,std::string configOrigin):value(value),configOrigin(configOrigin){};
};
class shortNameAliasCache{
    public:
    std::map<std::string,std::shared_ptr<alias>> namedAliases;
};

std::tuple<std::shared_ptr<Named_interface>,std::string> ResolveShortNameAlias(std::shared_ptr<SystemContext> ctx,std::string name);
void validateShortName(const std::string& name);
// void traverse_ptree_to_map(const boost::property_tree::ptree& pt, std::map<std::string, std::string>& result_map, const std::string& path = "");
std::tuple<std::shared_ptr<shortNameAliasConf>,std::shared_ptr<shortNameAliasCache>> loadShortNameAliasConf(std::string confPath);
std::shared_ptr<Named_interface> parseShortNameValue(const std::string& name);
std::shared_ptr<shortNameAliasCache> newShortNameAliasCache(std::string path,std::shared_ptr<shortNameAliasConf> conf);
#endif // SYSREgistriesV2_SHORTNAMES_H)
