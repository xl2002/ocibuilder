#include "filesys/system/unshare_linux.h"
#include <boost/compute/detail/getenv.hpp>
/**
 * @brief Get the user UID in parent user namespace
 * 
 * @return int User UID, default 0
 * @note Reads from "_CONTAINERS_ROOTLESS_UID" environment variable if set
 */
int GetRootlessUID() {
    const char* uidEnv = boost::compute::detail::getenv("_CONTAINERS_ROOTLESS_UID");
    if (uidEnv != nullptr && std::string(uidEnv) != "") {
        return std::stoi(uidEnv);
    }
    // return getuid();  // Get current user's UID
    return 0;
}
