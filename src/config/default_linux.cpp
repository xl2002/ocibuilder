#include "config/default_linux.h"
#include "filesys/system/selinux/selinux.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>


/**
 * @brief 获取默认的锁类型
 * @return 返回默认锁类型字符串("shm")
 */
std::string getDefaultLockType(){
    return "shm";
}
/**
 * @brief 获取Libpod临时目录路径
 * @return 返回默认的Libpod临时目录路径("/run/libpod")
 */
std::string getLibpodTmpDir(){
    return "/run/libpod";
}

/**
 * @brief 获取默认的机器挂载卷配置
 * @return 返回默认挂载卷配置向量(包含"$HOME:$HOME")
 */
std::vector<std::string> getDefaultMachineVolumes(){
    return std::vector<std::string>{"$HOME:$HOME"};

}
/**
 * @brief 获取默认的进程限制配置
 * @details 返回当前进程的nproc限制，格式为ulimits\n
 * 注意：nfile有时不能设置为无限制，限制硬编码为(oldMaxSize) 1048576 (2^20)\n
 * 在无特权容器中，这将失败，进程将使用其当前的限制\n
 * 参见：http://stackoverflow.com/a/1213069/1811501
 * @return 返回进程限制配置向量
 */
std::vector<std::string> getDefaultProcessLimits() {
#ifndef _WIN32
    struct rlimit rlim = {1048576, 1048576};  // 设置初始的硬编码值
    struct rlimit oldrlim = rlim;
    std::vector<std::string> defaultLimits;
    // 尝试将文件限制和进程限制设置为操作系统中的pid_max
    std::ifstream file("/proc/sys/kernel/pid_max");
    if (file.is_open()) {
        std::string val;
        std::getline(file, val);
        boost::algorithm::trim_right_if(val, boost::is_any_of("\n"));
        try {
            rlim_t max = boost::lexical_cast<rlim_t>(val);
            rlim = {max, max};
        } catch (const boost::bad_lexical_cast&) {
            // 忽略错误
        }
    }
    // 使用 const rlimit* 类型传递给 setrlimit
    if (setrlimit(RLIMIT_NPROC, &rlim) == 0) {
        defaultLimits.push_back("nproc=" + std::to_string(rlim.rlim_cur) + ":" + std::to_string(rlim.rlim_max));
    } else if (setrlimit(RLIMIT_NPROC, &oldrlim) == 0) {
        defaultLimits.push_back("nproc=" + std::to_string(oldrlim.rlim_cur) + ":" + std::to_string(oldrlim.rlim_max));
    }
#else
    struct rlimit{
        rlim_t rlim_cur;
        rlim_t rlim_max;
    };
    struct rlimit rlim = {1048576, 1048576};  // 设置初始的硬编码值
    struct rlimit oldrlim = rlim;
    std::vector<std::string> defaultLimits;
    // Windows系统暂不支持此功能
    defaultLimits.push_back("nproc=" + std::to_string(oldrlim.rlim_cur) + ":" + std::to_string(oldrlim.rlim_max));
#endif
    return defaultLimits;
    // return std::vector<std::string>{"nproc=4194304:4194304"};
}
/**
 * @brief 检查SELinux是否启用
 * @return true-SELinux已启用, false-SELinux未启用
 */
bool selinuxEnabled(){
    return GetEnabled();
}
// const std::string
// 	// OverrideContainersConfig holds the default config path overridden by the root user
// 	OverrideContainersConfig = "/etc/";

// 	// DefaultContainersConfig holds the default containers config path
// const std::string DefaultContainersConfig = "/usr/share/" + _configPath;

// 	// DefaultSignaturePolicyPath is the default value for the
// 	// policy.json file.
// const std::string DefaultSignaturePolicyPath = "/etc/containers/policy.json";
