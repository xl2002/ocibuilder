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
#include "storage/types/options.h"
#include "image/util/util.h"
#include "utils/cli/cli/common.h"
#include <stdlib.h> 
#include "filesys/systems.h"
#include <mutex>
#include <cstring>
#include <iostream>
#include "utils/common/go/file.h"
#include "utils/logger/ProcessSafeLogger.h"
#include <boost/compute/detail/getenv.hpp>
#include <pthread.h>
// #define PATH_MAX 4096
using std::string;

/**
 * @brief 检查是否已经设置了默认配置文件
 * 
 */
std::once_flag defaultStoreOptionsFlag;
std::once_flag defaultloadoptions;


std::mutex storesLock;

/**
 * @brief 重新加载配置文件内容到存储选项
 * @param configFile 配置文件路径
 * @param storeOptions 存储选项指针，用于接收加载的配置
 */
void ReloadConfigurationFile(const std::string& configFile, std::shared_ptr<StoreOptions> storeOptions) {
    // 实现加载配置文件的逻辑
    // 这部分需要根据实际情况来实现
}

/**
 * @brief 检查并重新加载配置文件(如果需要)
 * @param configFile 配置文件路径
 * @param storeOptions 存储选项指针，用于接收加载的配置
 * @return bool 如果配置文件已重新加载返回true，否则返回false
 */
bool ReloadConfigurationFileIfNeeded(const std::string& configFile, StoreOptions& storeOptions) {
    std::lock_guard<std::mutex> lock(prevReloadConfig.mutex);

    boost::filesystem::file_status fs;
    fs=boost::filesystem::status(configFile);
    if(fs.type()!=boost::filesystem::regular_file) return false;
    // try {
    //     fs = boost::filesystem::status(configFile);
    // } catch (const boost::filesystem::filesystem_error& e) {
    //     // 文件状态检查失败，返回错误
    //     throw myerror("检查配置文件状态失败: " + std::string(e.what()));
    // }
    auto mtime = boost::filesystem::last_write_time(configFile);

    if (prevReloadConfig.storeOptions && prevReloadConfig.mod == mtime && prevReloadConfig.configFile == configFile) {
        storeOptions = *prevReloadConfig.storeOptions;
        return true;
    }
    // try {
    //     // 重新加载配置文件
    //     if (!ReloadConfigurationFile(configFile, storeOptions)) {
    //         throw myerror("重新加载配置文件失败: " + configFile);
    //     }
    // } catch (const myerror& e) {
    //     // 重新加载配置文件失败，抛出错误
    //     throw myerror("重新加载配置文件失败: " + std::string(e.what()));
    // }

    prevReloadConfig.storeOptions = std::make_shared<StoreOptions>(storeOptions);
    prevReloadConfig.mod = mtime;
    prevReloadConfig.configFile = configFile;
    return true;

}
/**
 * @brief 设置存储选项的默认值
 * @param options 要设置默认值的存储选项引用
 */
void setDefaults(StoreOptions& options) {
        if (options.run_root.empty()) {
            options.run_root = defaultRunRoot;
        }
        if (options.graph_root.empty()) {
            options.graph_root = defaultGraphRoot;
        }
}
/**
 * @brief 加载默认存储选项
 * @details 从环境变量或默认位置加载存储配置，并设置默认值
 * @throws myerror 如果加载配置失败则抛出异常
 */
void loadDefaultStoreOptions() {
    try {
        defaultStoreOptions.graph_driver_name.clear();
        setDefaults(defaultStoreOptions);

        // 获取环境变量中的配置文件路径
        auto env_path = boost::compute::detail::getenv(storageConfEnv.c_str());
        std::string path = env_path ? std::string(env_path) : "";
        if (!path.empty()) {
            defaultOverrideConfigFile = path;
            if (!ReloadConfigurationFileIfNeeded(defaultOverrideConfigFile, defaultStoreOptions)) {
                logger->log_error("重新加载配置文件失败: " + defaultOverrideConfigFile);
                throw myerror("重新加载配置文件失败: " + defaultOverrideConfigFile);
            }
            setDefaults(defaultStoreOptions);
            return;
        }

        // 获取 XDG_CONFIG_HOME 环境变量
        auto xdg=boost::compute::detail::getenv("XDG_CONFIG_HOME");
        path=xdg?std::string(xdg):"";
        if (!path.empty()) {
            std::string homeConfigFile = path + "/containers/storage.conf";
            if (boost::filesystem::exists(homeConfigFile)) {
                defaultOverrideConfigFile = homeConfigFile;
            } else {
                logger->log_warning("XDG配置文件不存在: " + homeConfigFile);
                if (!boost::filesystem::exists(homeConfigFile)) {
                    throw myerror("无法访问配置文件: " + homeConfigFile);
                }
            }
        }

        // 检查覆盖的配置文件是否存在
        if (boost::filesystem::exists(defaultOverrideConfigFile)) {
            defaultConfigFile = defaultOverrideConfigFile;
            if (!ReloadConfigurationFileIfNeeded(defaultOverrideConfigFile, defaultStoreOptions)) {
                logger->log_error("重新加载覆盖配置文件失败: " + defaultOverrideConfigFile);
                throw myerror("重新加载配置文件失败: " + defaultOverrideConfigFile);
            }
            setDefaults(defaultStoreOptions);
            return;
        }

        // 默认配置文件路径
        if (!ReloadConfigurationFileIfNeeded(defaultConfigFile, defaultStoreOptions)) {
            if(boost::filesystem::exists(defaultConfigFile)) {
                logger->log_error("重新加载默认配置文件失败: " + defaultConfigFile);
                throw myerror("重新加载配置文件失败: " + defaultConfigFile);
            }
            //logger->log_warning("默认配置文件不存在: " + defaultConfigFile);
        }
        setDefaults(defaultStoreOptions);

    } catch (const myerror& e) {
        // 捕获并处理 myerror 异常，提供额外的错误信息
        logger->log_error("加载默认存储选项失败: "+std::string(e.what()));
        std::cerr << "加载默认存储选项时发生错误: " << e.what() << std::endl;
        throw; // 重新抛出异常，以便上层函数处理
    }
}
/**
 * @brief 扩展路径中的环境变量和特殊标记
 * @param path 要扩展的原始路径
 * @param rootlessUID 用于替换$UID标记的用户ID
 * @return std::string 扩展后的完整路径
 */
std::string expandEnvPath(const std::string& path, int rootlessUID) {
    std::string expandedPath = path;

    // 替换 $UID 为 rootlessUID
    boost::algorithm::replace_all(expandedPath, "$UID", std::to_string(rootlessUID));

    // 查找并扩展路径中的环境变量
    size_t pos = 0;
    while ((pos = expandedPath.find('$', pos)) != std::string::npos) {
        size_t end = expandedPath.find_first_of("/\\", pos + 1);
        std::string envVar = expandedPath.substr(pos + 1, end - pos - 1);
        std::string envValue = boost::compute::detail::getenv(envVar.c_str());

        if (!envValue.empty()) {
            expandedPath.replace(pos, end - pos, envValue);
        } else {
            logger->log_warning("环境变量 "+envVar+" 未定义");
            std::cerr << "环境变量 " << envVar << " 未定义。" << std::endl;
            // 继续搜索下一个环境变量
        }
        pos += envValue.length(); // 更新位置以继续搜索
    }

    try {
        // 解析符号链接
        expandedPath = boost::filesystem::canonical(expandedPath).string();
    } catch (const boost::filesystem::filesystem_error&) {
        // 如果解析符号链接失败，返回绝对路径
        expandedPath = boost::filesystem::absolute(expandedPath).string();
    }
    //去掉相对路径标志
    // expandedPath = boost::filesystem::canonical(expandedPath).string();
    // expandedPath = boost::filesystem::absolute(expandedPath).string();
    
    return expandedPath;
}
/**
 * @brief 获取环境变量值
 * @param name 环境变量名
 * @return std::string 环境变量值，如果未设置则返回空字符串
 */
std::string getEnv(const std::string& name) {
    auto value = boost::compute::detail::getenv(name.c_str());
    return value ? std::string(value) : std::string();
}
/**
 * @brief 获取rootless用户ID
 * @return int 用户ID，如果无法获取则返回0
 * @throws myerror 如果获取过程中发生错误则抛出异常
 */
int getRootlessUID() {
    try {
        // 获取环境变量
        std::string uidEnv = getEnv("_CONTAINERS_ROOTLESS_UID");
        if (!uidEnv.empty()) {
            // 将环境变量值转换为整数
            try {
                return std::stoi(uidEnv);
            } catch (const std::invalid_argument&) {
                // 捕获转换错误但不输出错误信息
                throw myerror("Invalid UID value in environment variable");
            } catch (const std::out_of_range&) {
                // 捕获转换范围错误但不输出错误信息
                throw myerror("UID value out of range");
            }
        }
        // 如果环境变量为空，Windows 上无法直接获取 UID，可以考虑其他方法
        // throw myerror("UID retrieval not supported on this platform");
        return 0;
    } catch (const myerror&) {
        // 仅捕获 myerror 类型的异常，不输出错误信息
        throw;
    } catch (...) {
        // 捕获其他异常，抛出 myerror 异常，不输出错误信息
        throw myerror("Unknown error occurred while retrieving UID");
    }
}
/**
 * @brief 默认的存储选项
 * 
 * @return StoreOptions 
 */
StoreOptions DefaultStoreOptions() {
    StoreOptions result;
    try {
        auto loadoptions = [&result]() {
            try {
                result = loadStoreOptions();
                storeptions=result;
            } catch (const myerror& e) {
                throw; // 重新抛出 myerror 类型的异常
            }
        };
        std::call_once(defaultloadoptions, loadoptions);
    } catch (const myerror& e) {
        throw; // 重新抛出 myerror 类型的异常
    }
    return storeptions;
}
/**
 * @brief 获取默认存储选项
 * @return StoreOptions 默认存储选项对象
 * @throws myerror 如果加载默认选项失败则抛出异常
 */
StoreOptions defaultOptions(){
    try{
        std::call_once(defaultStoreOptionsFlag, loadDefaultStoreOptions);
        return defaultStoreOptions;
    }catch(const myerror& e){
        throw;
    }
    
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
    try {
        // 检查是否已经设置了默认配置文件
        if (defaultConfigFileSet) {
            return defaultConfigFile;
        }

        // 获取环境变量中的配置文件路径
        auto envPath = boost::compute::detail::getenv(storageConfEnv.c_str());
        if (envPath != nullptr) {
            return std::string(envPath);
        }

        // 检查是否使用了用户存储
        if (!usePerUserStorage()) {
            if (boost::filesystem::exists(defaultOverrideConfigFile)) {
                return defaultOverrideConfigFile;
            }
            return defaultConfigFile;
        }

        // 获取 XDG_CONFIG_HOME 环境变量
        auto configHome = boost::compute::detail::getenv("XDG_CONFIG_HOME");
        if (configHome != nullptr) {
            return (boost::filesystem::path(configHome) / "containers" / "storage.conf").string();
        }

        // 获取用户主目录
        std::string home = boost::filesystem::path(boost::compute::detail::getenv("HOME")).string();
        if (home.empty()) {
            throw myerror("无法确定用户的主目录");
        }

        return (boost::filesystem::path(home) / ".config" / "containers" / "storage.conf").string();

    } catch (const std::exception& e) {
        throw myerror("获取默认配置文件路径失败: " + std::string(e.what()));
    }
}


/**
 * @brief 获取用户家目录对象
 * 该函数用于获取用户家目录的路径，并返回字符串类型的路径
 * 该函数并不直接返回实际的家目录路径，而是返回一个占位符"./"，
 * 实际的家目录路径需要通过实际的用户家目录获取方法替换该占位符
 * 
 * @return std::string 返回用户家目录路径
 */
/**
 * @brief 获取用户家目录路径
 * @return std::string 用户家目录路径
 */
std::string getHomeDir() {
    return "./"; // 替换为实际的用户家目录获取方法
}
/**
 * @brief 获取环境变量 XDG_CONFIG_HOME
 * 该函数用于获取环境变量 XDG_CONFIG_HOME 的值
 * 
 * @return std::string 
 */
/**
 * @brief 获取XDG配置目录路径
 * @return std::string XDG_CONFIG_HOME环境变量值或空字符串
 */
std::string getConfigHome() {
    return boost::compute::detail::getenv("XDG_CONFIG_HOME") ? boost::compute::detail::getenv("XDG_CONFIG_HOME") : "";
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
/**
 * @brief 检查是否使用用户个人存储
 * @return bool 是否使用用户个人存储
 */
bool usePerUserStorage() {
    return false; // 假设始终使用用户个人存储
}
/// @brief 包装了 std::call_once 调用，确保 loadDefaultStoreOptions 只执行一次。
/**
 * @brief 如果需要则加载默认存储选项
 * @return bool 加载是否成功
 */
bool loadDefaultStoreOptionsIfNeeded() {
    try {
        std::call_once(defaultStoreOptionsFlag, loadDefaultStoreOptions);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// 判断路径是否存在以及是否为目录
/**
 * @brief 检查目录是否存在
 * @param path 要检查的目录路径
 * @return bool 目录是否存在且为目录类型
 */
bool DirectoryExists(const std::string& path) {
    boost::filesystem::path p(path);
    return boost::filesystem::exists(p) && boost::filesystem::is_directory(p);
}
// 递归创建路径上的所有目录
/**
 * @brief 递归创建目录
 * @param path 要创建的目录路径
 * @return bool 创建是否成功
 */
bool MkdirAll(const std::string& path) {
    boost::filesystem::path p(path);

    // 如果目录已存在，则返回 true
    if (DirectoryExists(path)) {
        return true;
    }

    // 查找最后一个路径分隔符
    boost::filesystem::path parentPath = p.parent_path();

    // 递归创建父目录
    if (parentPath != p) { // 确保 parentPath 不是空路径
        if (!MkdirAll(parentPath.string())) {
            return false;
        }
    }

    // 创建当前目录
    try {
        return boost::filesystem::create_directory(p) || boost::filesystem::is_directory(p);
    } catch (const boost::filesystem::filesystem_error& e) {
        logger->log_error("创建目录失败: "+std::string(e.what()));
        std::cerr << "创建目录失败: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 加载存储选项，获取默认配置文件路径并加载存储选项。
 * 获取默认配置文件路径时，如果出现错误，则捕获并返回错误。
 * 加载存储选项时，使用获取到的配置文件路径。
 * 
 * @return StoreOptions 存储选项对象
 * @throws myerror 获取默认配置文件路径时出现错误
 */
/**
 * @brief 加载存储选项
 * @return StoreOptions 加载的存储选项对象
 * @throws myerror 如果加载失败则抛出异常
 */
StoreOptions loadStoreOptions() {
    StoreOptions result;
    try {
        // 获取默认配置文件
        std::string storageConf;
        try {
            storageConf = DefaultConfigFile();
        } catch (const myerror& e) {
            // 如果 DefaultConfigFile 抛出 myerror 异常，直接重新抛出
            throw;
        }

        // 从配置文件加载存储选项
        try {
            result = loadStoreOptionsFromConfFile(storageConf);
        } catch (const myerror& e) {
            // 如果 loadStoreOptionsFromConfFile 抛出 myerror 异常，直接重新抛出
            throw;
        }
    } catch (const myerror& e) {
        // 捕获并处理 myerror 异常，如果需要的话可以添加额外的处理逻辑
        throw; // 重新抛出异常
    }
    return result;
}
/**
 * @brief 从配置文件加载存储选项，配置文件路径为参数storageConf，
 * 并将存储选项的GraphRoot设置为storageConf，然后返回这个存储选项对象。
 * 
 * @param storageConf 配置文件路径
 * @return StoreOptions 存储选项对象
 */
/**
 * @brief 从配置文件加载存储选项
 * @param storageConf 配置文件路径
 * @return StoreOptions 加载的存储选项对象
 * @throws myerror 如果加载失败则抛出异常
 */
StoreOptions loadStoreOptionsFromConfFile(const std::string& storageConf) {
    StoreOptions storageOpts;
    std::string defaultRootlessRunRoot;
    std::string defaultRootlessGraphRoot;

    try {
        // 确保只加载一次默认存储选项
        std::call_once(defaultStoreOptionsFlag, loadDefaultStoreOptions);

        storageOpts = defaultStoreOptions;

        // 如果使用用户存储，则获取根目录存储选项
        if (usePerUserStorage()) {
            // storageOpts = getRootlessStorageOpts(storageOpts);
        }

        // 检查配置文件是否存在
        if (boost::filesystem::exists(storageConf)) {
            auto s=boost::filesystem::status(storageConf);
            if (s.type() == boost::filesystem::regular_file && !defaultConfigFileSet) {
                // defaultRootlessRunRoot = storageOpts.run_root;
                // defaultRootlessGraphRoot = storageOpts.graph_root;
                // storageOpts = StoreOptions{};
                // ReloadConfigurationFileIfNeeded(storageConf, &storageOpts);

                // // 如果配置文件没有指定图形根目录或运行根目录，设置合理的默认值
                // if (storageOpts.run_root.empty()) {
                //     storageOpts.run_root = defaultRootlessRunRoot;
                // }
                // if (storageOpts.graph_root.empty()) {
                //     storageOpts.graph_root = !storageOpts.rootless_storage_path.empty() 
                //                                 ? storageOpts.rootless_storage_path 
                //                                 : defaultRootlessGraphRoot;
                // }
            }
        }

        // 确保 run_root 被设置
        if (storageOpts.run_root.empty()) {
            throw myerror("run_root 必须被设置");
        }

        // 处理环境变量路径
        uint32_t rootlessUID = getRootlessUID();
        try {
            storageOpts.run_root = expandEnvPath(storageOpts.run_root, rootlessUID);
        } catch (const myerror& e) {
            throw myerror("扩展 run_root 路径失败: " + std::string(e.what()));
        }

        if (storageOpts.graph_root.empty()) {
            throw myerror("graph_root 必须被设置");
        }
        try {
            storageOpts.graph_root = expandEnvPath(storageOpts.graph_root, rootlessUID);
        } catch (const myerror& e) {
            throw myerror("扩展 graph_root 路径失败: " + std::string(e.what()));
        }

        if (!storageOpts.rootless_storage_path.empty()) {
            try {
                storageOpts.rootless_storage_path = expandEnvPath(storageOpts.rootless_storage_path, rootlessUID);
            } catch (const myerror& e) {
                throw myerror("扩展 rootless_storage_path 路径失败: " + std::string(e.what()));
            }
        }

        if (!storageOpts.image_store.empty() && storageOpts.image_store == storageOpts.graph_root) {
            throw myerror("image_store " + storageOpts.image_store + " 必须未设置或不同于 graph_root");
        }

    } catch (const myerror& e) {
        // 捕获并处理 myerror 异常，提供额外的错误信息
        throw myerror("加载配置文件时发生错误: " + std::string(e.what()));
    }

    return storageOpts;
}
/**
 * @brief 获取或创建存储实例
 * @param options 存储选项
 * @return std::shared_ptr<Store> 存储实例指针
 * @throws myerror 如果创建存储失败则抛出异常
 */
std::shared_ptr<Store> GetStore(StoreOptions options) {
    try {
        // 初始化默认配置
        StoreOptions defaultOpts = defaultOptions();
        
        StoreOptions finalOptions = options;

        // 如果没有完全填充选项，则使用默认值
        if (options.run_root.empty() && options.graph_root.empty() && 
            options.graph_driver_name.empty() && options.graph_driver_options.empty()) {
            finalOptions = defaultOpts;
        }

        // 获取绝对路径
        if (!finalOptions.graph_root.empty()) {
            finalOptions.graph_root = boost::filesystem::absolute(finalOptions.graph_root).string();
        }

        if (!finalOptions.run_root.empty()) {
            finalOptions.run_root = boost::filesystem::absolute(finalOptions.run_root).string();
        }

        // 加锁
        std::mutex stores_lock;
        std::lock_guard<std::mutex> lock(stores_lock);

        // 查找已有的 store
        for (const auto& s : stores) {
            if ((s->graph_root == finalOptions.graph_root) && 
                (s->run_root == finalOptions.run_root) &&
                (finalOptions.graph_driver_name.empty() || s->graph_driver_name == finalOptions.graph_driver_name)) {
                return s;
            }
        }

        // 检查根路径
        if (finalOptions.run_root.empty() && finalOptions.graph_root.empty()) {
            throw myerror("未指定存储运行根目录或图形根目录");
        }
        if (finalOptions.graph_root.empty()) {
            finalOptions.graph_root = defaultOpts.graph_root;
        }
        if (finalOptions.run_root.empty()) {
            finalOptions.run_root = defaultOpts.run_root;
        }

        // 创建相关目录
        boost::filesystem::create_directories(finalOptions.run_root);
        //去掉符号链接，并且符合win的标准
        finalOptions.run_root = boost::filesystem::canonical(finalOptions.run_root).make_preferred().string();
        boost::filesystem::create_directories(finalOptions.graph_root);
        finalOptions.graph_root = boost::filesystem::canonical(finalOptions.graph_root).make_preferred().string();

        if (!finalOptions.image_store.empty()) {
            boost::filesystem::create_directories(finalOptions.image_store);
        }
        boost::filesystem::create_directories(finalOptions.graph_root + "/" + finalOptions.graph_driver_name);

        // 初始化锁文件
        auto graph_lock = GetLockFile(finalOptions.graph_root + "/storage.lock");
        auto userns_lock = GetLockFile(finalOptions.graph_root + "/userns.lock");

        // 设置自动命名空间大小
        uint32_t auto_ns_min_size = finalOptions.auto_ns_min_size ? finalOptions.auto_ns_min_size : AutoUserNsMinSize;
        uint32_t auto_ns_max_size = finalOptions.auto_ns_max_size ? finalOptions.auto_ns_max_size : AutoUserNsMaxSize;

        // 创建 store 实例
        auto s = std::make_shared<Store>();
        s->run_root = finalOptions.run_root;
        s->graph_driver_name = finalOptions.graph_driver_name;
        s->graph_driver_priority = finalOptions.graph_driver_priority;
        s->graph_lock = graph_lock;
        s->userns_lock = userns_lock;
        s->graph_root = finalOptions.graph_root;
        s->graph_options = finalOptions.graph_driver_options;
        s->image_store_dir = finalOptions.image_store;
        s->pull_options = finalOptions.pull_options;
        s->auto_userns_user = finalOptions.root_auto_ns_user;
        s->auto_ns_min_size = auto_ns_min_size;
        s->auto_ns_max_size = auto_ns_max_size;
        s->disable_volatile = finalOptions.disable_volatile;
        s->transient_store = finalOptions.transient_store;
        if(s->image_store_dir.empty()){ 
            s->image_store_dir = s->GetImageStoragePath();
        }
        // 加载 store
        s->load();

        // 添加到已创建的 store 列表中
        stores.push_back(s);

        return s;

    } catch (const myerror& e) {
        throw; // 重新抛出 myerror 类型的异常
    }
}

// 将 std::string 转换为 std::wstring
/**
 * @brief 将std::string转换为std::wstring
 * @param s 要转换的字符串
 * @return std::wstring 转换后的宽字符串
 */
std::wstring s2ws(const std::string& s) {
    return std::wstring(s.begin(), s.end());
}
