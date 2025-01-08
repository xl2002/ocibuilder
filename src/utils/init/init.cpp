#include "utils/init/init.h"

/**
 * @brief 初始化各个命令
 * 
 * 1. 设置命令行名称为程序名称
 * 2. 允许参数和选项交错使用
 * 3. 对标志进行排序
 * 4. 初始化构建命令
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