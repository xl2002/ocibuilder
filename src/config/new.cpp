#include "config/new.h"

#include <mutex>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <stdlib.h>   // For getenv
#include "config/defaut.h"
#include "config/default_linux.h"
#include "utils/common/go/file.h"
#include "utils/logger/logrus/exported.h"
#include <boost/any.hpp>
// #include <boost/align.hpp>

std::mutex cachedConfigMutex;
/**
 * @brief 创建新的容器配置对象
 * @param options 配置选项指针，可为nullptr使用默认选项
 * @return std::shared_ptr<Config> 新创建的配置对象
 * @details 
 * - 如果options为nullptr，会创建默认选项
 * - 如果options->SetDefault为true，会缓存配置作为默认配置
 * - 内部调用newLocked()完成实际配置创建
 * - 线程安全，使用mutex保护缓存操作
 * @throws myerror 如果配置创建失败
 */
std::shared_ptr<Config> New(std::shared_ptr<Options> options) {
    if (!options) {
        options = std::make_shared<Options>();
    } else if (options->SetDefault) {
        cachedConfigMutex.lock();
        // 使用锁后立即创建一个作用域退出的锁定对象以确保自动解锁
        std::lock_guard<std::mutex> lock(cachedConfigMutex, std::adopt_lock);
    }
    try
    {
        return newLocked(options);
    }
    catch(const  myerror& e)
    {
        throw;
    }
    
    
}
/**
 * @brief 获取默认容器配置
 * @return std::shared_ptr<Config> 默认配置对象
 * @details
 * - 如果默认配置未设置，会通过New()创建并缓存新配置
 * - 返回的配置应被视为只读，修改会影响所有调用方
 * - 线程安全，使用mutex保护缓存操作
 * @throws myerror 如果配置创建失败
 */
std::shared_ptr<Config> Config_defaut() {
    std::lock_guard<std::mutex> lock(cachedConfigMutex);
    if(cachedConfig!=nullptr){
        return cachedConfig;
    }else if(cachedConfigError!=nullptr){
        throw *cachedConfigError;
    }
    try {
        auto op=std::make_shared<Options>();
        op->SetDefault=true;
        cachedConfig = newLocked(op);
        return cachedConfig;
    } catch (const myerror& e) {
        // cachedConfigError = std::make_shared<myerror>(e.what());
        throw;
    }
}

/**
 * @brief 创建并初始化新的配置对象(内部使用)
 * @param options 配置选项
 * @return std::shared_ptr<Config> 初始化后的配置对象
 * @details
 * - 从默认配置开始
 * - 加载系统配置并合并
 * - 处理模块配置
 * - 处理环境变量覆盖
 * - 验证并设置环境
 * - 如果options->SetDefault为true，会缓存配置
 * @throws myerror 如果配置创建或验证失败
 */
std::shared_ptr<Config> newLocked(std::shared_ptr<Options> options) {
    // 使用内置默认值开始
    std::shared_ptr<Config> config;
    try {
        config = defaultConfig();
    } catch (const myerror& e) {
        throw;
    }

    // 收集系统配置并在需要时进行合并
    std::vector<std::string> configs;
    boost::system::error_code ec;
    
    std::tie(configs, ec) = systemConfigs();
    
    if(ec) {
        throw myerror("finding config on system: "+ ec.message());
    }
    // for (const auto& path : configs) {
    //     // 合并后续配置中的更改到之前的配置中
    //     // 每个配置文件指定的字段将覆盖先前的字段
    //     // boost::optional<myerror> err = readConfigFromFile(path, config, true);
    //     if (err) {
    //         throw myerror("reading system config "+ path +": " +*err->what());
    //     }
    //     // std::cout << "Merged system config " << path << std::endl;
    // }

    std::vector<std::string> modules;
    try {
        modules = options->modules();
    } catch (const myerror& e) {
        throw;
    }
    config->loadedModules = modules;

    options->additionalConfigs.insert(options->additionalConfigs.end(), modules.begin(), modules.end());

    // _OVERRIDE 变量必须始终获胜。这是我们需要遵守的约定（用于 Podman CI）
    const char* envPath = boost::compute::detail::getenv(containersConfOverrideEnv.c_str());
    if (envPath && std::string(envPath) != "") {
        std::ifstream file(envPath);
        if (!file.good()) {
            throw myerror(containersConfOverrideEnv+" file bad");
        }
        options->additionalConfigs.push_back(envPath);
    }

    // 如果调用者指定了配置路径，则读取它以覆盖系统默认值
    // for (const auto& add : options->additionalConfigs) {
    //     if (add.empty()) {
    //         continue;
    //     }
    //     boost::optional<myerror> err = readConfigFromFile(add, config, false);
    //     if (err) {
    //         throw myerror("reading additional config" +add +": " +*err->what());
    //     }
    //     std::cout << "Merged additional config " << add << std::endl;
    // }

    config->addCAPPrefix();
    config->Validate();
    config->setupEnv();

    if (options->SetDefault) {
        cachedConfig = config;
        cachedConfigError.reset();
    }
    return config;
}
/**
 * @brief 获取系统级配置文件路径列表
 * @return std::tuple<std::vector<std::string>, boost::system::error_code> 
 *         配置文件路径列表和错误码
 * @details
 * - 检查containersConfEnv环境变量指定的配置文件
 * - 添加默认系统配置文件路径
 * - 添加系统配置目录(.d)中的配置文件
 * - 添加用户配置文件路径
 * - 添加用户配置目录(.d)中的配置文件
 * - 返回的路径已按字母顺序排序
 */
std::tuple<std::vector<std::string>, boost::system::error_code> systemConfigs() {
    std::vector<std::string> configs;
    boost::system::error_code finalErr;

    // 获取环境变量
    const char* envPath = boost::compute::detail::getenv("containersConfEnv");
    if (envPath != nullptr) {
        // 检查路径是否存在
        if (!fileExists(envPath)) {
            finalErr = boost::system::errc::make_error_code(boost::system::errc::no_such_file_or_directory);
            return std::make_tuple(std::vector<std::string>(), finalErr);
        }
        configs.push_back(envPath);
        return std::make_tuple(configs, finalErr);
    }

    // 添加默认配置
    configs.push_back(DefaultContainersConfig);
    configs.push_back(OverrideContainersConfig);
    // std::cout<<DefaultSignaturePolicyPath<<std::endl;

    // 调用 addConfigs 函数添加其他配置文件

    auto config1 = addConfigs(OverrideContainersConfig+".d", configs);
    // if (config1.second) {
    //     return std::make_tuple(std::vector<std::string>(), config1.second);
    // }
    configs=config1.first;
    // 获取用户配置路径
    std::string path;
    boost::system::error_code ec;
    std::tie(path, ec) = userConfigPath();
    if (ec) {
        return std::make_tuple(std::vector<std::string>(), ec);
    }
    configs.push_back(path);

    // 调用 addConfigs 函数添加用户配置文件
    auto conf = addConfigs(path + ".d", configs);
    // if(conf.second) {
    //     return std::make_tuple(std::vector<std::string>(), conf.second);
    // }
    configs=conf.first;

    return std::make_tuple(configs, finalErr);
}

namespace fs = boost::filesystem;
/**
 * @brief 从指定目录添加配置文件到配置列表
 * @param dirPath 要扫描的目录路径
 * @param configs 现有的配置文件路径列表
 * @return std::pair<std::vector<std::string>, boost::system::error_code>
 *         更新后的配置文件列表和错误码
 * @details
 * - 扫描指定目录下的.conf文件
 * - 忽略子目录
 * - 按字母顺序排序找到的文件
 * - 合并到现有配置列表
 * - 如果目录不存在，返回原列表
 */
std::pair<std::vector<std::string>,  boost::system::error_code>
addConfigs(const std::string& dirPath, std::vector<std::string>& configs) {
    std::vector<std::string> newConfigs;
    boost::system::error_code ec;

    // 检查目录是否存在
    if (!fs::exists(dirPath, ec)) {
        if (ec) {
            return {configs, ec};  // 目录不存在或其他错误
        }
        return {configs, ec};  // 目录不存在，但没有错误
    }

    // 遍历目录
    for (fs::directory_iterator it(dirPath, ec), end; it != end; it.increment(ec)) {
        if (ec) {
            return {configs, ec};  // 遍历时遇到错误
        }

        const fs::path& path = it->path();
        if (fs::is_directory(path)) {
            // 忽略子目录
            continue;
        }

        // 仅添加以 `.conf` 结尾的文件
        if (boost::algorithm::iends_with(path.string(), ".conf")) {
            newConfigs.push_back(path.string());
        }
    }

    // 对文件路径排序
    std::sort(newConfigs.begin(), newConfigs.end());

    // 合并新配置文件到原始配置列表
    configs.insert(configs.end(), newConfigs.begin(), newConfigs.end());
    return {configs, ec};
}

/**
 * @brief 获取用户级配置文件路径
 * @return std::tuple<std::string, boost::system::error_code>
 *         用户配置文件路径和错误码
 * @details
 * - 优先检查XDG_CONFIG_HOME环境变量
 * - 如果未设置，则使用HOME/USERPROFILE环境变量
 * - 在Windows和Linux上使用不同的环境变量
 * - 返回路径格式为: [config_dir]/_configPath
 */
std::tuple<std::string, boost::system::error_code> userConfigPath() {
    std::string configPath;
    boost::system::error_code ec;

    // 获取环境变量 XDG_CONFIG_HOME
    const char* configHome = boost::compute::detail::getenv("XDG_CONFIG_HOME");
    if (configHome != nullptr && boost::nowide::utf::strlen(configHome) > 0) {
        // 如果 XDG_CONFIG_HOME 设置了，则使用它
        configPath = (boost::filesystem::path(configHome) / "_configPath").string();
    } else {
        // 否则使用用户的 HOME 目录
        #ifdef __linux__
        auto home = boost::compute::detail::getenv("HOME");
        #else
        auto home = boost::compute::detail::getenv("USERPROFILE");
        #endif
        // auto home = boost::compute::detail::getenv("HOME");
        if (home==nullptr || boost::nowide::utf::strlen(home) == 0) {
            std::cerr << "Environment variable HOME is empty or not set." << std::endl;
            return std::make_tuple("", ec);
        }
        std::string homeDir = boost::filesystem::path(home).string();
        if (homeDir.empty()) {
            ec = boost::system::errc::make_error_code(boost::system::errc::no_such_file_or_directory);
            return std::make_tuple("", ec);
        }
        configPath = (boost::filesystem::path(homeDir) / UserOverrideContainersConfig).string();
    }

    return std::make_tuple(configPath, ec);
}

// 读取指定路径的配置文件，并尝试将其内容解析到 Config 中。
// config 参数指定之前的默认配置。
// 如果 path 中的 TOML 文件只指定了一些字段，则 config 参数中的默认值将用于所有其他字段。
// boost::optional<myerror> readConfigFromFile(const std::string& path, std::shared_ptr<Config> config, bool ignoreErrNotExist) {
//     boost::optional<myerror> err;
//     boost::system::error_code ec;
//     // 输出日志信息，表示正在读取配置文件
//     // std::cout << "正在读取配置文件: " << path << std::endl;
//     // Tracef("Reading config from " + path);
//     // 检查文件是否存在
//     if (!boost::filesystem::exists(path,ec)) {
//         if (ignoreErrNotExist && ec == boost::system::errc::no_such_file_or_directory) {
//             return boost::none; // 返回成功，无错误
//         }
//         return myerror("decode configuration "+path+": "+ec.message());
//     }

    
//     // 解析 TOML 文件
//     boost::property_tree::ptree pt;
//     boost::property_tree::read_json(path, pt);

//     // 将解析后的内容填充到 Config 结构体中
//     // 这里你需要根据实际的 Config 结构体字段进行解析

//     // 检查是否有未解析的键
//     // if (pt.empty()) {
//     //     return myerror("Failed to decode the keys from"+path);
//     // }

//     return boost::none; // 返回成功，无错误
// }
// 解析TOML配置文件
// void parseConfigFile(const std::string& confPath, boost::any conf) {
//     boost::property_tree::ptree pt;

//     // 尝试读取文件
//     try {
//         boost::property_tree::ini_parser::read_ini(confPath, pt);
//     } catch (const boost::property_tree::ini_parser_error& e) {
//         throw myerror("无法解析配置文件: " + confPath + ": " + e.what());
//     }

//     // 从ptree中提取值到conf中，需根据实际字段映射
//     // 例如:
//     // conf.someField = pt.get<std::string>("Section.someField");
// }
// Find the specified modules in the options. Return an error if a specific
// module cannot be located on the host.
/**
 * @brief 获取配置选项中的模块路径列表
 * @return std::vector<std::string> 模块路径列表
 * @details
 * - 如果Modules为空，返回空列表
 * - 否则解析模块路径并返回
 * - 模块路径可能来自环境变量或配置选项
 * - 实际实现中会检查模块是否存在
 * @note 当前实现仅返回空列表，完整实现需解析模块路径
 */
std::vector<std::string> Options::modules() {
    std::vector<std::string> configs;
    boost::system::error_code ec;

    if (Modules.empty()) {
        return std::vector<std::string>(); // 返回空列表和无错误
    }
    return std::vector<std::string>();
    // std::vector<std::string> dirs;
    // try {
    //     dirs = ModuleDirectories();
    // } catch (const std::exception& e) {
    //     ec = boost::system::errc::make_error_code(boost::system::errc::io_error);
    //     return {configs, ec};
    // }

    // for (const auto& path : o.Modules) {
    //     try {
    //         std::string resolved = resolveModule(path, dirs);
    //         configs.push_back(resolved);
    //     } catch (const std::runtime_error& e) {
    //         ec = boost::system::errc::make_error_code(boost::system::errc::no_such_file_or_directory);
    //         return {configs, ec};
    //     }
    // }

    // return {configs, ec}; // 返回模块路径列表和可能的错误
}
