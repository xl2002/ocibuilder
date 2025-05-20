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
std::shared_ptr<Command> rootcmd; 
/**
 * @brief 初始化所有命令行命令
 * @details 执行流程：
 *          1. 初始化根命令（rootcmd）
 *          2. 依次注册各子命令模块：
 *             - build/push/pull 等核心操作
 *             - config/version 等辅助命令
 *             - login/logout 等认证命令
 * @warning 该函数不应直接调用，应通过 `initialize_cmd()` 保证线程安全
 * @see initialize_cmd()
 */
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
/**
 * @brief 线程安全的命令行初始化入口
 * @details 使用 std::call_once 确保全局命令对象只初始化一次
 * @par 示例：
 * @code
 * initialize_cmd();  // 首次调用执行初始化
 * initialize_cmd();  // 后续调用无副作用
 * @endcode
 * @relates rootcmd
 */
void initialize_cmd(){
    std::call_once(flag_cmd, init_command);
}
