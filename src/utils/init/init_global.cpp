#include "utils/init/init_global.h"
std::once_flag flag_global;

void init_global(){
    init_cmd();
    init_config();
    init_filesys();
    init_image();
    init_network();
    init_security();
    init_storage();
    init_utils();
}
void initialize_global(){
    std::call_once(flag_global, init_global);
}
