/**
 * @file utils.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief utils.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-07-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "storage/utils.h"
#include "pkg/util/util.h"
/**
 * @brief 
 * 
 */
const string overlayDriver  = "overlay";
const string overlay2       = "overlay2";
const string storageConfEnv = "CONTAINERS_STORAGE_CONF";
/**
 * @brief 检查是否已经设置了默认配置文件
 * 
 */
bool defaultConfigFileSet = false;
/// @brief defaultConfigFile 系统范围的 storage.conf 文件的路径
std::string defaultConfigFile = "D:/share/containers/storage.conf";
// std::string storageConfEnv = "STORAGE_CONF_ENV";
/// @brief 
std::string defaultOverrideConfigFile = "D:/containers/storage.conf";
StoreOptions defaultStoreOptions;
/**
 * @brief 默认的存储选项
 * 
 * @return StoreOptions 
 */
StoreOptions DefaultStoreOptions(){
    StoreOptions ret;
    try{
        ret=loadStoreOptions();
    }catch(const myerror& e){
        throw ;
    }
    return ret;
}
/**
 * @brief 实现 DefaultConfigFile 函数
 * 
 * @return std::string 返回config文件路径
 */
std::string DefaultConfigFile() {
    // 检查是否已经设置了默认配置文件
    if (defaultConfigFileSet) {
        return defaultConfigFile;
    }

    // 通过环境变量获取路径
    const char* path = std::getenv(storageConfEnv.c_str());
    if (path) {
        return path;
    }

    // 如果不使用用户个人存储，则检查默认覆盖配置文件
    if (!usePerUserStorage()) {
        if (fileExists(defaultOverrideConfigFile)) {
            return defaultOverrideConfigFile;
        }
        return defaultConfigFile;
    }

    // 检查 XDG_CONFIG_HOME 环境变量
    std::string configHome = getConfigHome();
    if (!configHome.empty()) {
        return configHome + "/containers/storage.conf";
    }

    // 获取用户家目录并拼接默认配置文件路径
    std::string home = getHomeDir();
    if (home.empty()) {
        throw myerror("cannot determine user's homedir");
    }
    return home + "/containers/storage.conf";
}

/**
 * @brief 获取主目录对象
 * 
 * @return std::string 
 */
std::string getHomeDir() {
    return "D:/"; // 替换为实际的用户家目录获取方法
}
/**
 * @brief 获取环境变量 XDG_CONFIG_HOME
 * 
 * @return std::string 
 */
std::string getConfigHome() {
    return std::getenv("XDG_CONFIG_HOME") ? std::getenv("XDG_CONFIG_HOME") : "";
}
/**
 * @brief 判断是否使用用户个人存储
 * 
 * @return true 
 * @return false 
 */
bool usePerUserStorage() {
    return true; // 假设始终使用用户个人存储
}
/**
 * @brief 实现 loadStoreOptions 函数
 * 
 * @return std::string 
 */
StoreOptions loadStoreOptions() {
    // 获取默认配置文件路径
    std::string storageConf;
    try {
        storageConf = DefaultConfigFile();
    } catch (const myerror& e) {
        // 捕获并返回错误
        throw myerror(e.what());
        // return defaultStoreOptions;
    }
    // 加载存储选项
    return loadStoreOptionsFromConfFile(storageConf);
}
/**
 * @brief 加载存储选项
 * 
 * @param storageConf 
 * @return StoreOptions 
 */
StoreOptions loadStoreOptionsFromConfFile(const std::string& storageConf){
    defaultStoreOptions.GraphRoot=storageConf;
    return defaultStoreOptions;
}