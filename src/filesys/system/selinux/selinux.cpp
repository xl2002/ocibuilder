#include "filesys/system/selinux/selinux.h"
#include "filesys/system/selinux/selinux_linux.h"
bool GetEnabled(){
    return getEnabled();
}
void SetDisabled(){
    setDisabled();
}