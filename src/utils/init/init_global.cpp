#include "utils/init/init_global.h"
std::once_flag flag_global;

void init_global(){
    init_utils();
    
    init_config();
    init_filesys();
    init_cmd();
    init_image();
    init_network();
    init_security();
    init_storage();

}
void initialize_global(){
    std::call_once(flag_global, init_global);
}
