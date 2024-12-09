#include "image/shortnames/sysregistriesv2/system_registries_v2.h"
#include "filesys/utils/homedir_unix.h"
#include "image/shortnames/sysregistriesv2/shortnames.h"

std::mutex configMutex;
std::shared_ptr<configWrapper> newConfigWrapper(std::shared_ptr<SystemContext> ctx) {
    return newConfigWrapperWithHomeDir(ctx,homedir::Get());
}

std::shared_ptr<configWrapper> newConfigWrapperWithHomeDir(std::shared_ptr<SystemContext> ctx,std::string homeDir) {
    configWrapper wrapper;
    std::string userRegistriesFilePath = homeDir + "/" + userRegistriesFile;
    std::string userRegistriesDirPath = homeDir + "/" + userRegistriesDir;

    // 根据用户路径或系统文件决定 configPath
    if (ctx != nullptr && !ctx->SystemRegistriesConfPath.empty()) {
        wrapper.configPath = ctx->SystemRegistriesConfPath;
    } else {
        struct stat buffer;
        if (stat(userRegistriesFilePath.c_str(), &buffer) == 0) {
            // 用户的 registries.conf 存在，不读取系统目录
            // 从 ctx 或用户目录返回配置目录
            wrapper.configPath = userRegistriesFilePath;
            if (ctx != nullptr && !ctx->SystemRegistriesConfDirPath.empty()) {
                wrapper.configDirPath = ctx->SystemRegistriesConfDirPath;
            } else {
                wrapper.userConfigDirPath = userRegistriesDirPath;
            }
            return std::make_shared<configWrapper>(wrapper);
        } else if (ctx != nullptr && !ctx->RootForImplicitAbsolutePaths.empty()) {
            wrapper.configPath = ctx->RootForImplicitAbsolutePaths + "/" + builtinRegistriesConfPath;
        } else {
            wrapper.configPath = builtinRegistriesConfPath;
        }
    }

    // 如果不使用用户配置文件，则可能使用系统和用户目录
    if (ctx != nullptr && !ctx->SystemRegistriesConfDirPath.empty()) {
        // 明确选择的目录：仅使用该目录
        wrapper.configDirPath = ctx->SystemRegistriesConfDirPath;
    } else if (ctx != nullptr && !ctx->RootForImplicitAbsolutePaths.empty()) {
        wrapper.configDirPath = ctx->RootForImplicitAbsolutePaths + "/" + builtinRegistriesConfPath;
        wrapper.userConfigDirPath = userRegistriesDirPath;
    } else {
        wrapper.configDirPath = builtinRegistriesConfPath;
        wrapper.userConfigDirPath = userRegistriesDirPath;
    }

    return std::make_shared<configWrapper>(wrapper);
}

std::shared_ptr<parsedConfig> getConfig(std::shared_ptr<SystemContext> ctx) {
    auto wrapper = newConfigWrapper(ctx);
    std::lock_guard<std::mutex> lock(configMutex);
    return tryUpdatingCache(ctx, wrapper);
}
std::shared_ptr<parsedConfig> tryUpdatingCache(std::shared_ptr<SystemContext> ctx, std::shared_ptr<configWrapper> wrapper) {
    std::lock_guard<std::mutex> lock(configMutex);
    auto config = loadConfigFile(wrapper->configPath, false);
    // auto dinConfigs=dropInConfigs(wrapper);
    // for(auto path:dinConfigs){
    //     auto dropIn=loadConfigFile(path,true);
    //     config->updateWithConfigurationFrom(dropIn);
    // }
    if(*config->shortNameMode==ShortNameMode::ShortNameModeInvalid){
        config->shortNameMode=std::make_shared<ShortNameMode>(ShortNameMode::ShortNameModePermissive);
    }
    if(config->partialV2->CredentialHelpers.empty()){
        config->partialV2->CredentialHelpers=std::vector<std::string>({"containers-auth.json"});
    }
    return config;
}
std::shared_ptr<parsedConfig> loadConfigFile(std::string path,bool forceV2) {

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(path, pt);
    auto combinedTOML=std::make_shared<V2RegistriesConf>();
    // std::map<std::string, std::string> config_map;
    traverse_ptree_to_map(pt, combinedTOML->UnqualifiedSearchRegistries);
    auto res=std::make_shared<parsedConfig>();
    res->partialV2=combinedTOML;
    res->unqualifiedSearchRegistriesOrigin=path;
    res->shortNameMode=std::make_shared<ShortNameMode>(ShortNameMode::ShortNameModeInvalid);
    auto cache=newShortNameAliasCache(path,res->partialV2);
    res->aliasCache=cache;
    return res;
}

std::map<std::string, std::string> UnqualifiedSearchRegistries(std::shared_ptr<SystemContext> ctx) {
    std::map<std::string, std::string> registries;
    std::tie(registries, std::ignore) = UnqualifiedSearchRegistriesWithOrigin(ctx);
    return registries;
}

std::tuple<std::map<std::string, std::string>, std::string> UnqualifiedSearchRegistriesWithOrigin(std::shared_ptr<SystemContext> ctx) {
    auto config = getConfig(ctx);
    return std::make_tuple(config->partialV2->UnqualifiedSearchRegistries, config->unqualifiedSearchRegistriesOrigin);
}