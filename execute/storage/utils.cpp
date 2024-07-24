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
#include "cli/common.h"
#include <stdlib.h> 
#include <windows.h>
#include <mutex>
#include <iostream>
using namespace std;
/**
 * @brief 
 * 
 */
const string overlayDriver  = "overlay";
const string overlay2       = "overlay2";
const string storageConfEnv = "CONTAINERS_STORAGE_CONF";
const string defaultRunRoot = "/run/containers/storage";
const string defaultGraphRoot = "/var/lib/containers/storage";
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
std::mutex storesLock;
vector<std::shared_ptr<store>> stores;
bool loadDefaultStoreOptions() {
    defaultStoreOptions.graph_driver_name = "";

    auto setDefaults = []() {
        if (defaultStoreOptions.run_root.empty()) {
            defaultStoreOptions.run_root = defaultRunRoot;
        }
        if (defaultStoreOptions.graph_root.empty()) {
            defaultStoreOptions.graph_root = defaultGraphRoot;
        }
    };
    setDefaults();

    std::string path = getenv("CONTAINERS_STORAGE_CONF");
    if (!path.empty()) {
        defaultOverrideConfigFile = path;
        if (!ReloadConfigurationFileIfNeeded(path, &defaultStoreOptions)) {
            return false;
        }
        setDefaults();
        return true;
    }

    std::string xdgConfigHome = getenv("XDG_CONFIG_HOME");
    if (!xdgConfigHome.empty()) {
        std::string homeConfigFile = joinPath(xdgConfigHome, "containers/storage.conf");
        if (fileExists(homeConfigFile)) {
            defaultOverrideConfigFile = homeConfigFile;
        }
    }

    if (fileExists(defaultOverrideConfigFile)) {
        defaultConfigFile = defaultOverrideConfigFile;
        if (!ReloadConfigurationFileIfNeeded(defaultOverrideConfigFile,&defaultStoreOptions)) {
            return false;
        }
        setDefaults();
        return true;
    }

    if (!fileExists(defaultConfigFile)) {
        std::cerr << "Attempting to use " << defaultConfigFile << std::endl;
    }
    if (!ReloadConfigurationFileIfNeeded(defaultConfigFile, &defaultStoreOptions) && std::make_error_code(std::errc::no_such_file_or_directory) != std::errc::no_such_file_or_directory) {
        return false;
    }
    setDefaults();
    return true;
}
/**
 * @brief 默认的存储选项
 * 
 * @return StoreOptions 
 */
StoreOptions DefaultStoreOptions() {
    StoreOptions ret;
    try{
        ret=loadStoreOptions();
    }catch(const myerror& e){
        throw ;
    }
    return ret;
}
/**
 * @brief 获取默认配置文件路径
 * 该函数用于获取默认的配置文件路径，优先级为：
 * 1. 检查是否已经设置了默认配置文件，如果设置则返回
 * 2. 通过环境变量获取路径，如果环境变量存在则返回
 * 3. 如果不使用用户个人存储，则检查默认覆盖配置文件，如果存在则返回
 * 4. 检查 XDG_CONFIG_HOME 环境变量，如果存在则返回
 * 5. 获取用户家目录并拼接默认配置文件路径，如果无法获取则抛出异常
 * 
 * @return std::string 返回默认配置文件路径
 */
std::string DefaultConfigFile() {
    ///<检查是否已经设置了默认配置文件
    if (defaultConfigFileSet) {
        return defaultConfigFile;
    }

    ///<通过环境变量获取路径
    const char* path = std::getenv(storageConfEnv.c_str());
    if (path) {
        return path;
    }

    ///<如果不使用用户个人存储，则检查默认覆盖配置文件
    if (!usePerUserStorage()) {
        if (fileExists(defaultOverrideConfigFile)) {
            return defaultOverrideConfigFile;
        }
        return defaultConfigFile;
    }

    ///<检查 XDG_CONFIG_HOME 环境变量
    std::string configHome = getConfigHome();
    if (!configHome.empty()) {
        return configHome + "/containers/storage.conf";
    }

    // 获取用户目录并拼接默认配置文件路径
    std::string home = getHomeDir();
    if (home.empty()) {
        throw myerror("cannot determine user's homedir");
    }
    return home + "/containers/storage.conf";
}


/**
 * @brief 获取用户家目录对象
 * 该函数用于获取用户家目录的路径，并返回字符串类型的路径
 * 该函数并不直接返回实际的家目录路径，而是返回一个占位符"D:/"，
 * 实际的家目录路径需要通过实际的用户家目录获取方法替换该占位符
 * 
 * @return std::string 返回用户家目录路径
 */
std::string getHomeDir() {
    return "D:/"; // 替换为实际的用户家目录获取方法
}
/**
 * @brief 获取环境变量 XDG_CONFIG_HOME
 * 该函数用于获取环境变量 XDG_CONFIG_HOME 的值
 * 
 * @return std::string 
 */
std::string getConfigHome() {
    return std::getenv("XDG_CONFIG_HOME") ? std::getenv("XDG_CONFIG_HOME") : "";
}
/**
 * @brief 检查是否使用用户个人存储
 * 
 * 该函数用于检查是否应该使用用户个人存储。由于目前的实现中始终返回 true，
 * 因此始终使用用户个人存储。
 * 
 * @return true 始终返回 true，表示使用用户个人存储
 * @return false 永远不会返回 false，因为当前的实现始终返回 true
 */
bool usePerUserStorage() {
    return true; // 假设始终使用用户个人存储
}

/**
 * @brief 加载存储选项，获取默认配置文件路径并加载存储选项。
 * 获取默认配置文件路径时，如果出现错误，则捕获并返回错误。
 * 加载存储选项时，使用获取到的配置文件路径。
 * 
 * @return StoreOptions 存储选项对象
 * @throws myerror 获取默认配置文件路径时出现错误
 */
StoreOptions loadStoreOptions() {
    ///<获取默认配置文件路径
    std::string storageConf;
    try {
        storageConf = DefaultConfigFile();
    } catch (const myerror& e) {
        ///<捕获并返回错误
        throw myerror(e.what());
        ///<return defaultStoreOptions;
    }
    ///<加载存储选项
    return loadStoreOptionsFromConfFile(storageConf);
}
/**
 * @brief 从配置文件加载存储选项，配置文件路径为参数storageConf，
 * 并将存储选项的GraphRoot设置为storageConf，然后返回这个存储选项对象。
 * 
 * @param storageConf 配置文件路径
 * @return StoreOptions 存储选项对象
 */
StoreOptions loadStoreOptionsFromConfFile(const std::string& storageConf){
    defaultStoreOptions.graph_root=storageConf;
    return defaultStoreOptions;
}
// 将 std::string 转换为 std::wstring
std::wstring s2ws(const std::string& s) {
    std::wstring ws(s.begin(), s.end());
    return ws;
}

// 判断路径是否存在以及是否为目录
bool DirectoryExists(const std::string& path) {
    std::wstring wpath = s2ws(path);
    DWORD attribs = GetFileAttributesW(wpath.c_str());
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}
// 递归创建路径上的所有目录
bool MkdirAll(const std::string& path) {
    if (DirectoryExists(path)) {
        return true;
    }

    // 查找最后一个路径分隔符
    size_t pos = path.find_last_of("\\/");
    if (pos != std::string::npos) {
        std::string parentPath = path.substr(0, pos);

        // 递归创建父目录
        if (!MkdirAll(parentPath)) {
            return false;
        }
    }

    // 创建当前目录
    std::wstring wpath = s2ws(path);
    if (!CreateDirectoryW(wpath.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        std::cerr << "Error creating directory: " << path << std::endl;
        return false;
    }
    return true;
}
shared_ptr<store> GetStore(StoreOptions options){
    if(loadDefaultStoreOptions()){
        cerr << "Error loading default store options" << endl;
        return nullptr;
    }
    StoreOptions storeOptions = options;
    if (storeOptions.run_root.empty() && storeOptions.graph_root.empty() && storeOptions.graph_driver_name.empty() && storeOptions.graph_driver_options.empty()) {
        storeOptions = defaultStoreOptions;
    }

    ///<处理路径
     if (!storeOptions.graph_root.empty()) {
        storeOptions.graph_root = Abspath(storeOptions.graph_root);
    }
    if (!storeOptions.run_root.empty()) {
        storeOptions.run_root = Abspath(storeOptions.run_root);
    }
    lock_guard<mutex> lock(storesLock);

    for (const auto& s : stores) {
        if (s->graph_root == storeOptions.graph_root && s->run_root == storeOptions.run_root &&
            (storeOptions.graph_driver_name.empty() || s->graph_driver_name == storeOptions.graph_driver_name)) {
            return s;
        }
    }

    if (storeOptions.run_root.empty() || storeOptions.graph_root.empty()) {
        cerr << "Error: runroot or graphroot must be set" << endl;
        return nullptr;
    }

    // 创建目录
     if (!MkdirAll(storeOptions.run_root)) {
        cerr << "Error creating run root directory: " << storeOptions.run_root << endl;
        return nullptr;
    }
    if (!MkdirAll(storeOptions.graph_root)) {
        cerr << "Error creating graph root directory: " << storeOptions.graph_root << endl;
        return nullptr;
    }

    // 创建新的 Store 对象
    auto newStore = std::make_shared<store>();
    newStore->run_root = storeOptions.run_root;
    newStore->graph_root = storeOptions.graph_root;
    newStore->graph_driver_name = storeOptions.graph_driver_name;
    newStore->graph_options = storeOptions.graph_driver_options;

    // 将新创建的 Store 对象添加到 stores 列表中
    stores.push_back(newStore);

    return newStore;
}
bool  ReloadConfigurationFileIfNeeded(string configFile, StoreOptions* storeOptions){
    return true;
}