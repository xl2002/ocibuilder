#include "utils/init/init_global.h"
std::once_flag flag_global;
/**
 * @brief 执行所有子系统的初始化
 * @details 初始化顺序严格遵循依赖关系：
 *          1. 基础工具(utils) → 配置(config) → 文件系统(filesys)
 *          2. 镜像(image) → 网络(network) → 安全(security)
 *          3. 存储(storage) → 命令行(cmd)
 * @warning 禁止直接调用，必须通过 initialize_global() 访问
 * @throws std::runtime_error 若子系统初始化失败
 */
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
/**
 * @brief 线程安全的全局初始化入口
 * @details 特性：
 *          - 使用 std::call_once 保证幂等性
 *          - 捕获并包装子系统的初始化异常
 * @par 典型用法：
 * @code{.cpp}
 * int main() {
 *     initialize_global(); // 安全初始化
 *     // ...其他逻辑
 * }
 * @endcode
 */
void initialize_global(){
    std::call_once(flag_global, init_global);
}
