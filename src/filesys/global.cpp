#include "utils/init/init_global.h"
#include "filesys/graphdriver/driver.h"
#include "filesys/system/cgroups_linux.h"
#include "filesys/platforms/platforms.h"
#include "filesys/platforms/default_unix.h"
#include "filesys/system/selinux/selinux_linux.h"
#include "storage/storage/overlay.h"
// 全局变量定义
std::string ErrNotSupported;
std::string ErrPrerequisites;
std::string ErrIncompatibleFS;
std::string ErrLayerUnknown;

std::unordered_map<std::string, std::function<std::shared_ptr<Driver>(const std::string&, const driver_Options&)>> drivers;

bool isCgroupV2;
boost::optional<std::error_code> isCgroupV2Err;

std::regex specifierRe;

std::string readOnlyFileLabel;
std::shared_ptr<selinuxState> state;
bool haveThreadSelf;
std::string policyRootVal;
std::map<std::string, std::string> labels;

std::string GOOS;
std::string GOARCH;
/**
 * @brief 初始化filesys
 * 
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
    #ifdef _WIN32
        GOOS = "windows";
    #else
        //#include <unistd.h>
        GOOS = "linux"; // 假设默认是 Linux，可以根据实际系统修改
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