#include "utils/init/init_cmd.h"
#include "cmd/root/root.h"
std::once_flag flag_cmd;
Command rootcmd; 
void init_command(){
    rootcmd=init_rootcmd(); ///<定义rootcmd，作为全局变量
    init_buildcmd();
    init_push();
}
void initialize_cmd(){
    std::call_once(flag_cmd, init_command);
}
