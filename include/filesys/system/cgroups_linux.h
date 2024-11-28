#ifndef FILESYS_SYSTEM_CGROUPS_LINUX_H
#define FILESYS_SYSTEM_CGROUPS_LINUX_H
#include <mutex>
#include <system_error>
#include <tuple>
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/vfs.h> // 或者 <sys/statfs.h> 在Linux/Unix系统上
#endif
#include <boost/optional.hpp>
// 模拟cgroup v2相关的全局变量
extern bool isCgroupV2;
extern boost::optional<std::error_code> isCgroupV2Err;
#ifdef _WIN32
// Windows上没有cgroup v2，我们可以返回一个默认值
std::tuple<bool, boost::optional<std::error_code>> Enabled();
#elif defined __linux
std::tuple<bool, boost::optional<std::error_code>> Enabled();
#endif

#endif // CGROUPS_CGROUPS_LINUX_H)
