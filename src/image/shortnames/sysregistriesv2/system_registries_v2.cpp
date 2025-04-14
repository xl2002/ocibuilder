/**
 * @file system_registries_v2.cpp
 * @brief 实现系统注册表V2配置相关功能
 */
#include "image/shortnames/sysregistriesv2/system_registries_v2.h"
#include "filesys/utils/homedir_unix.h"
#include "image/shortnames/sysregistriesv2/shortnames.h"

std::mutex configMutex;
/**
 * @brief 创建新的配置包装器
 * @param ctx 系统上下文，可为nullptr
 * @return 返回初始化好的配置包装器
 * @details 
 * 使用用户主目录调用newConfigWrapperWithHomeDir
 */
std::shared_ptr<configWrapper> newConfigWrapper(std::shared_ptr<SystemContext> ctx) {
    return newConfigWrapperWithHomeDir(ctx,homedir::Get());
}

/**
 * @brief 使用指定主目录创建配置包装器
 * @param ctx 系统上下文，可为nullptr
 * @param homeDir 用户主目录路径
 * @return 返回初始化好的配置包装器
 * @details 
 * 1. 检查系统上下文中的配置路径
 * 2. 检查用户注册表配置文件是否存在
 * 3. 根据条件设置配置路径和目录路径
 */
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

/**
 * @brief 获取系统配置
 * @param ctx 系统上下文，可为nullptr
 * @return 返回解析后的配置对象
 * @details 
 * 1. 创建新的配置包装器
 * 2. 锁定配置互斥锁
 * 3. 尝试更新缓存
 */
std::shared_ptr<parsedConfig> getConfig(std::shared_ptr<SystemContext> ctx) {
    auto wrapper = newConfigWrapper(ctx);
    std::lock_guard<std::mutex> lock(configMutex);
    return tryUpdatingCache(ctx, wrapper);
}
/**
 * @brief 尝试更新配置缓存
 * @param ctx 系统上下文，可为nullptr
 * @param wrapper 配置包装器
 * @return 返回更新后的配置对象
 * @details 
 * 1. 加载主配置文件
 * 2. 设置默认短名称模式
 * 3. 设置默认凭证助手
 */
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
/**
 * @brief 加载配置文件
 * @param path 配置文件路径
 * @param forceV2 是否强制使用V2格式
 * @return 返回解析后的配置对象
 * @details 
 * 1. 创建V2注册表配置对象
 * 2. 初始化配置对象
 * 3. 创建短名称别名缓存
 */
std::shared_ptr<parsedConfig> loadConfigFile(std::string path,bool forceV2) {

    // boost::property_tree::ptree pt;
    // boost::property_tree::ini_parser::read_ini(path, pt);
    auto combinedTOML=std::make_shared<V2RegistriesConf>();
    // std::map<std::string, std::string> config_map;
    // traverse_ptree_to_map(pt, combinedTOML->UnqualifiedSearchRegistries);
    auto res=std::make_shared<parsedConfig>();
    res->partialV2=combinedTOML;
    res->unqualifiedSearchRegistriesOrigin=path;
    res->shortNameMode=std::make_shared<ShortNameMode>(ShortNameMode::ShortNameModeInvalid);
    auto cache=newShortNameAliasCache(path,res->partialV2);
    res->aliasCache=cache;
    return res;
}

/**
 * @brief 获取未限定搜索注册表
 * @param ctx 系统上下文，可为nullptr
 * @return 返回注册表名称到URL的映射
 * @details 
 * 调用UnqualifiedSearchRegistriesWithOrigin并忽略来源信息
 */
std::map<std::string, std::string> UnqualifiedSearchRegistries(std::shared_ptr<SystemContext> ctx) {
    std::map<std::string, std::string> registries;
    std::tie(registries, std::ignore) = UnqualifiedSearchRegistriesWithOrigin(ctx);
    return registries;
}

/**
 * @brief 获取未限定搜索注册表及其来源
 * @param ctx 系统上下文，可为nullptr
 * @return 返回tuple包含:
 *   - map: 注册表名称到URL的映射
 *   - string: 配置来源路径
 * @details 
 * 1. 获取系统配置
 * 2. 返回注册表映射和来源路径
 */
std::tuple<std::map<std::string, std::string>, std::string> UnqualifiedSearchRegistriesWithOrigin(std::shared_ptr<SystemContext> ctx) {
    auto config = getConfig(ctx);
    return std::make_tuple(config->partialV2->UnqualifiedSearchRegistries, config->unqualifiedSearchRegistriesOrigin);
}
