#include "filesys/system/cgroups_linux.h"
std::once_flag isCgroupV2Once;

#ifdef _WIN32
// Windows上没有cgroup v2，我们可以返回一个默认值
std::tuple<bool, boost::optional<std::error_code>> Enabled() {
    std::call_once(isCgroupV2Once, []() {
        isCgroupV2 = false; // Windows不支持cgroup v2
        isCgroupV2Err = std::make_error_code(std::errc::not_supported);
    });
    return std::make_tuple(isCgroupV2, isCgroupV2Err);
}
#elif defined __linux
// Linux/Unix上的实现
constexpr long CGROUP2_SUPER_MAGIC = 0x63677270;

std::tuple<bool, boost::optional<std::error_code>> Enabled() {
    std::call_once(isCgroupV2Once, []() {
        struct statfs st;
        if (statfs("/sys/fs/cgroup", &st) != 0) {
            isCgroupV2 = false;
            isCgroupV2Err = std::error_code(errno, std::generic_category());
        } else {
            isCgroupV2 = (st.f_type == CGROUP2_SUPER_MAGIC);
            isCgroupV2Err = std::error_code(); // 无错误
        }
    });
    return std::make_tuple(isCgroupV2, isCgroupV2Err);
}
#endif