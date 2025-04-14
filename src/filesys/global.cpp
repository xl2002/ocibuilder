#include "utils/init/init_global.h"
#include "filesys/graphdriver/driver.h"
#include "filesys/system/cgroups_linux.h"
#include "filesys/platforms/platforms.h"
#include "filesys/platforms/default_unix.h"
#include "filesys/system/selinux/selinux_linux.h"
#include "storage/storage/overlay.h"
/// @file global.cpp
/// @brief 文件系统全局变量和初始化函数定义

/// @var ErrNotSupported
/// @brief 驱动不支持错误消息

/// @var ErrPrerequisites
/// @brief 驱动前置条件不满足错误消息

/// @var ErrIncompatibleFS
/// @brief 不兼容的文件系统错误消息

/// @var ErrLayerUnknown
/// @brief 未知层错误消息

/// @var drivers
/// @brief 文件系统驱动注册表，键为驱动名称，值为驱动初始化函数

/// @var isCgroupV2
/// @brief 标记是否使用cgroup v2

/// @var isCgroupV2Err
/// @brief cgroup v2检测错误码

/// @var specifierRe
/// @brief 用于验证名称的正则表达式

/// @var readOnlyFileLabel
/// @brief 只读文件标签

/// @var state
/// @brief SELinux状态对象

/// @var haveThreadSelf
/// @brief 标记是否支持线程本地存储

/// @var policyRootVal
/// @brief SELinux策略根目录

/// @var labels
/// @brief 系统标签映射表

/// @var GOOS
/// @brief 操作系统标识(linux/windows等)

/// @var GOARCH
/// @brief 系统架构标识(amd64/arm等)
/**
 * @brief 初始化文件系统全局环境
 * 
 * 该函数负责初始化文件系统相关的全局变量和状态，包括：
 * - 错误消息字符串初始化
 * - 注册 overlay 文件系统驱动
 * - 检测 cgroup 版本
 * - 初始化 SELinux 状态
 * - 设置操作系统和架构标识
 * 
 * @note 该函数应在程序启动时尽早调用
 * @warning 多次调用可能导致全局状态不一致
 */
void init_filesys(){
    ErrNotSupported = "driver not supported";
    ErrPrerequisites = "prerequisites for driver not satisfied (wrong filesystem?)";
    ErrIncompatibleFS = "backing file system is unsupported for this graph driver";
    ErrLayerUnknown = "unknown layer";
    // Separator = '/';
    // 注册 overlay 驱动程序
    drivers["overlay"] = [](const std::string& home, const driver_Options& options) -> std::shared_ptr<Driver> {
        return std::make_shared<Driver>(Init(home, options));
    };
    
    isCgroupV2=false;
    isCgroupV2Err=std::error_code();
    specifierRe.assign("[a-zA-Z0-9]+");

    state=std::make_shared<selinuxState>();
    haveThreadSelf = false;
    #ifdef __linux__
        //#include <unistd.h>
        GOOS = "linux"; // 假设默认是 Linux，可以根据实际系统修改
    #else
        GOOS = "windows";
    #endif

    #if defined(__x86_64__) || defined(_M_X64)
        GOARCH = "amd64";
    #elif defined(__i386__) || defined(_M_IX86)
        GOARCH = "386";
    #elif defined(__arm__) || defined(_M_ARM)
        GOARCH = "arm";
    #elif defined(__aarch64__)
        GOARCH = "arm64";
    #else
        GOARCH = "unknown";
    #endif
}
