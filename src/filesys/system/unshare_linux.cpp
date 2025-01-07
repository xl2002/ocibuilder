#include "filesys/system/unshare_linux.h"
#include <boost/compute/detail/getenv.hpp>
// GetRootlessUID 返回父用户命名空间中的用户UID
int GetRootlessUID() {
    const char* uidEnv = boost::compute::detail::getenv("_CONTAINERS_ROOTLESS_UID");
    if (uidEnv != nullptr && std::string(uidEnv) != "") {
        return std::stoi(uidEnv);
    }
    // return getuid();  // 获取当前用户的UID
    return 0;
}
