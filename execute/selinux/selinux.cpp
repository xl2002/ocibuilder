#include "selinux/selinux.h"
#include "selinux/selinux_linux.h"
bool GetEnabled(){
    return getEnabled();
}
void SetDisabled(){
    setDisabled();
}