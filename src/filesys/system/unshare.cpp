#include "filesys/system/unshare.h"
/**
 * @brief Get the home directory path for current user
 * 
 * @return std::string Home directory path string
 * @note On Linux returns "/root", on other platforms returns ".\\oci_images"
 */
std::string HomeDir(){
    #if defined(__linux__)
        return std::string("/root");
    #else
        return std::string(".\\oci_images");
    #endif
}
