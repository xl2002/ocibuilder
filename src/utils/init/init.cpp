#include "utils/init/init.h"

/**
 * @brief 应用主初始化函数 
 * @param argc 参数个数，来自main函数
 * @param argv 参数数组，来自main函数
 * @details 执行流程：
 *          1. 调用全局初始化（initialize_global）
 *          2. 配置基础命令行参数：
 *             - 设置程序名称（从argv[0]获取）
 *             - 启用参数/选项交错模式（interspersed）
 *             - 启用标志排序（SortedFlags）
 *          3. 初始化命令系统（initialize_cmd）
 * @par 典型用法：
 * @code{.cpp}
 * int main(int argc, char** argv) {
 *     init(argc, argv); // 主初始化
 *     // ...其他逻辑
 * }
 * @endcode
 * @warning 需确保在调用任何命令功能前执行
 * @see initialize_global()
 * @see initialize_cmd()
 */
void init(int argc, char const *argv[]){
    initialize_global();
    // 设置命令行名称为程序名称
    CommandLine->name=argv[0];
    // 允许参数和选项交错使用
    CommandLine->interspersed=true;
    // 对标志进行排序
    CommandLine->SortedFlags=true;
    // 初始化构建命令
    // init_buildcmd();
    
    initialize_cmd();
}
void Delete(){
    if(CommandLine!=nullptr){
        delete CommandLine;
        CommandLine=nullptr;
    }
}