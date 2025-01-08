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
#include "cmd/root/root.h"
#include "utils/init/init.h"
#include "utils/cli/cobra/flag.h"
#include "utils/cli/cobra/command.h"
// #include "cmd/build/build.h"
#include <iostream>
// #include "utils/logger/logrus/exported.h"
using namespace std;


/**
 * @brief 工具的执行入口
 * <p>
 * 1. rootcmd 实例化
 * 2. 初始化各个命令
 * 3. 执行rootcmd
 * @param argc main函数的参数数量
 * @param argv 参数列表,argv[0]为根命令
 * @return int  0表示正常退出
 * 
 */
int main(int argc, char const *argv[]){
    // 1. rootcmd 实例化
    // 2. 初始化各个命令
    init(argc, argv);
    
    try
    {
        // 3. 执行rootcmd
        rootcmd.Execute(argc, argv);
    }
    catch(const myerror& e)
    {
        // 4.捕捉异常
        e.logerror();
        exit(1);
    }
    // rootcmd.Run();
    return 0;
}
