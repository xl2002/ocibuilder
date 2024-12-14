#ifndef FILESYS_SYSTEM_CGROUPS_LINUX_H
#define FILESYS_SYSTEM_CGROUPS_LINUX_H
#include <mutex>
#include <system_error>
#include <tuple>
#include "filesys/systems.h"

#include <boost/optional.hpp>
// 模拟cgroup v2相关的全局变量
extern bool isCgroupV2;
extern boost::optional<std::error_code> isCgroupV2Err;

#ifdef __linux__
std::tuple<bool, boost::optional<std::error_code>> Enabled();
#else
std::tuple<bool, boost::optional<std::error_code>> Enabled();
#endif
#endif // CGROUPS_CGROUPS_LINUX_H)
