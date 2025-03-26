#include "utils/init/init_cmd.h"
#include "cmd/root/root.h"
#include "cmd/build/build.h"
#include "cmd/push/push.h"
#include "cmd/config/config.h"
#include "cmd/version/version.h"
#include "cmd/pull/pull.h"
#include "cmd/tag/tag.h"
#include "cmd/rmi/rmi.h"
#include "cmd/inspect/inspect.h"
#include "cmd/images/images.h"
#include "cmd/pull/pull.h"
#include "cmd/info/info.h"
#include "cmd/login/login.h"
#include "cmd/logout/logout.h"
#include "cmd/save/save.h"
std::once_flag flag_cmd;
Command rootcmd; 
void init_command(){
    rootcmd=init_rootcmd(); ///<定义rootcmd，作为全局变量
    init_buildcmd();
    init_push();
    init_configcmd();
    init_version();
    init_pull();
    init_tag();
    init_rmi();
    init_inspect();
    init_images();
    init_info();
    init_login();
    init_logout();
    init_save();
}
void initialize_cmd(){
    std::call_once(flag_cmd, init_command);
}
