#include "filesys/system/selinux/selinux.h"
#include "filesys/system/selinux/selinux_linux.h"
/**
 * @brief Check if SELinux is enabled
 * 
 * @return bool True if SELinux is enabled, false otherwise
 */
bool GetEnabled(){
    return getEnabled();
}
/** 
 * @brief Disable SELinux
 */
void SetDisabled(){
    setDisabled();
}
