#include "utils/init/init_cmd.h"
std::once_flag flag_cmd;

void init_command(){
    init_buildcmd();
}
void initialize_cmd(){
    std::call_once(flag_cmd, init_command);
}
