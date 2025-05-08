#include "utils/init/init_global.h"
#include "storage/storage/storage_transport.h"
#include "storage/types/options.h"
#include "storage/lockfile/lastwrite.h"
#include "storage/lockfile/lockfile.h"
#include "storage/storage/overlay.h"
#include "storage/types/options.h"
#include "utils/logger/ProcessSafeLogger.h"
// 全局变量定义
std::shared_ptr<StoreTransport_interface> Transport;

std::string overlayDriver;
std::string overlay2;
std::string storageConfEnv;

std::atomic<uint64_t> lastWriterIDCounter;
size_t lastWriterIDSize;

std::map<std::string, std::shared_ptr<lockFile>> lockFiles;

std::string mountProgramFlagFile;

std::string defaultConfigFile;
std::string defaultOverrideConfigFile;
std::string defaultRunRoot;
std::string defaultGraphRoot;
std::string defaultImagestore;
std::string defaultlayerpath;
int AutoUserNsMinSize;
int AutoUserNsMaxSize;
bool defaultConfigFileSet;
StoreOptions storeptions;
StoreOptions defaultStoreOptions;
vector<std::shared_ptr<Store>> stores;
ReloadConfig prevReloadConfig;
std::shared_ptr<ProcessSafeLogger> logger;
std::string log_dir;
std::string log_prefix;


/**
 * @brief 初始化storage模块的全局变量
 * @details 该函数初始化容器存储相关的全局配置变量，包括：
 *          - 存储传输接口
 *          - overlay驱动配置
 *          - 存储配置文件路径
 *          - 用户命名空间大小限制
 *          - 默认存储路径等
 * @note 会根据操作系统类型(linux/windows)设置不同的默认路径格式
 * @warning 该函数应在程序启动时尽早调用
 */
void init_storage(){
    Transport= std::make_shared<storageTransport>();

    overlayDriver  = "overlay";
    overlay2       = "overlay2";
    storageConfEnv = "CONTAINERS_STORAGE_CONF";

    lastWriterIDCounter.store(0);
    lastWriterIDSize=64;

    mountProgramFlagFile = ".has-mount-program";
    #ifdef __linux__
    /// @brief defaultConfigFile 系统范围的 storage.conf 文件的路径
    defaultConfigFile = "./oci_images/storage.conf";
    // std::string storageConfEnv = "STORAGE_CONF_ENV";
    /// @brief 
    defaultOverrideConfigFile = "./storage.conf";
    defaultRunRoot = "./oci_images";
    // const string defaultRunRoot = "/run/containers/storage";
    defaultGraphRoot = "./oci_images";
    defaultImagestore="/oci_registry";
    defaultlayerpath="/"+overlayDriver;
    // const string defaultGraphRoot = "/var/lib/containers/storage";
    #else
    /// @brief defaultConfigFile 系统范围的 storage.conf 文件的路径
    defaultConfigFile = ".\\oci_images\\storage.conf";
    // std::string storageConfEnv = "STORAGE_CONF_ENV";
    /// @brief 
    defaultOverrideConfigFile = ".\\storage.conf";
    defaultRunRoot = ".\\oci_images";
    defaultImagestore="\\oci_registry";
    defaultlayerpath="\\"+overlayDriver;
    // const string defaultRunRoot = "/run/containers/storage";
    defaultGraphRoot = ".\\oci_images";
    // const string defaultGraphRoot = "/var/lib/containers/storage";
    #endif
    AutoUserNsMinSize=1024;
    AutoUserNsMaxSize=65536;
    defaultConfigFileSet = false;
    log_dir=defaultRunRoot+"/log";
    log_prefix="ociBuild";
    logger=Newlogger();
}
