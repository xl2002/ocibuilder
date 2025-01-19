/**
 * @file options.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief options.h 声明与可选项option相关的结构和方法
 * @version 0.1
 * @date 2024-07-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#if !defined(STORAGE_TYPES_OPTIONS_H)
#define STORAGE_TYPES_OPTIONS_H
#include <string>
#include <map>
#include <vector>
#include "utils/common/error.h"
#include "storage/storage/storage.h"
#include <cstdlib>   // For getenv
#include <sys/stat.h> // For stat
// #include <stdexcept> // For runtime_error
#include <fstream>   // For ifstream
#include <sstream>   // For stringstream
#include <memory>
#include <boost/compute/detail/getenv.hpp>
#include <boost/filesystem.hpp>
#include <boost/process/environment.hpp>
#include <boost/system/error_code.hpp>
#include <boost/algorithm/string/replace.hpp>
using std::string;
using std::map;
using std::vector;
/**
 * @brief 记录镜像存储方面的信息
 * 
 */
class Store;
class StoreOptions{
    public:
    // 文件系统路径，用于存储运行时信息，如活动挂载点的位置，这些信息在主机重启后将丢失。
    string run_root;
    // 文件系统路径，用于存储层、镜像和容器的内容。
    string graph_root;
    // 如果需要与容器存储分开的位置，这是镜像存储的替代位置。
    string image_store;
    // string image_store;
    // 无根用户的存储路径，默认为 $HOME/.local/share/containers/storage。
    string rootless_storage_path;
    // 如果未指定驱动程序，将从 GraphDriverPriority 或平台相关的优先级列表中选择最适合的驱动程序。
    string graph_driver_name;
    // 要尝试初始化 Store_interface 的存储驱动程序列表，除非设置了 GraphDriverName。
    // 该列表可用于定义尝试驱动程序的自定义顺序。
    vector<string> graph_driver_priority;
    // 驱动程序特定的选项。
    vector<string> graph_driver_options;
    // 用于在使用 UID 映射的用户命名空间内设置容器的根文件系统。
    // vector<IDMap> uid_map;
    // vector<IDMap> gid_map;

    // 用于在自动设置 root 用户的用户命名空间时选择子范围的用户。
    string root_auto_ns_user;
    // 自动用户命名空间的最小大小。
    uint32_t auto_ns_min_size;
    // 自动用户命名空间的最大大小。
    uint32_t auto_ns_max_size;
    // 提供给拉取管理器的选项。
    map<string, string> pull_options;
    // 设置时不允许易失性挂载。
    bool disable_volatile;
    // 如果为瞬态，则在引导时不保留容器（在 runroot 中存储数据库）。
    bool transient_store;
};
class ReloadConfig {
    public:
    std::shared_ptr<StoreOptions> storeOptions=std::make_shared<StoreOptions>();
    std::time_t mod;  // Use std::time_t to store modification time
    std::mutex mutex;
    std::string configFile;
};
extern string overlayDriver;
extern string overlay2;
extern string storageConfEnv;
//函数声明
StoreOptions defaultOptions();
bool loadDefaultStoreOptionsIfNeeded();
StoreOptions DefaultStoreOptions();
bool usePerUserStorage();
std::string getConfigHome();
std::string getHomeDir();
// bool fileExists(const std::string& filename);
std::string DefaultConfigFile();
StoreOptions loadStoreOptionsFromConfFile(const std::string& storageConf);
StoreOptions loadStoreOptions();
///<ReloadConfigurationFileIfNeeded这个函数负责一些初始化变量的赋值
void loadDefaultStoreOptions(); 
bool MkdirAll(const std::string& path);
std::wstring s2ws(const std::string& s);
bool DirectoryExists(const std::string& path);

bool ReloadConfigurationFileIfNeeded(const std::string& configFile, StoreOptions* storeOptions);
///<ReloadConfigurationFileIfNeeded这个函数负责一些初始化变量的赋值
std::shared_ptr<Store> GetStore(StoreOptions options);

extern std::string defaultConfigFile;
extern std::string defaultOverrideConfigFile;
extern std::string defaultRunRoot;
extern std::string defaultGraphRoot;
extern std::string defaultImagestore;
extern std::string defaultlayerpath;
extern int AutoUserNsMinSize;
extern int AutoUserNsMaxSize;
extern bool defaultConfigFileSet;
extern StoreOptions storeptions;
extern StoreOptions defaultStoreOptions;
extern vector<std::shared_ptr<Store>> stores;
extern ReloadConfig prevReloadConfig;

#endif // MACRO