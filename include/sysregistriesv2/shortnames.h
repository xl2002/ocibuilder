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
};
struct shortNameAliasCache{
    std::map<std::string,std::shared_ptr<alias>> namedAliases;
};

std::tuple<std::shared_ptr<Named_interface>,std::string> ResolveShortNameAlias(std::shared_ptr<SystemContext> ctx,std::string name);
void validateShortName(const std::string& name);



#endif // SYSREgistriesV2_SHORTNAMES_H)
