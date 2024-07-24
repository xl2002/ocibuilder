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
#include "main/root.h"
#include "cobra/flag.h"
#include "cobra/command.h"
#include "main/build.h"
#include <iostream>

using namespace std;

/**
 * @brief 初始化各个命令
 * 
 * 1. 设置命令行名称为程序名称
 * 2. 允许参数和选项交错使用
 * 3. 对标志进行排序
 * 4. 初始化构建命令
 */
void init(int argc, char const *argv[]){
    // 设置命令行名称为程序名称
    CommandLine->name=argv[0];
    // 允许参数和选项交错使用
    CommandLine->interspersed=true;
    // 对标志进行排序
    CommandLine->SortedFlags=true;
    // 初始化构建命令
    init_buildcmd();
}
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
