/**
 * @file main.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 工具程序执行入口
 * @version 0.1
 * @date 2024-06-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "root.h"
#include "flag.h"
#include "command.h"
#include "build.h"


using namespace std;

/**
 * @brief 初始化各个命令
 * 
 */
void init(){
    init_buildcmd();
}

/**
 * @brief 工具的执行入口
 * <p>
 * @param argc main函数的参数数量
 * @param argv 参数列表,argv[0]为根命令
 * @return int 
 * 
 */

int main(int argc, char const *argv[]){
    //创建rootcmd；初始化rootcmd；
    // Rootcmd* Appcmd=init_rootcmd();
    
    // Command build=init_buildcmd();///build
    init();
    return 0;
}
