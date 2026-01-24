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
#include "utils/logger/ProcessSafeLogger.h"
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
    try
    {    
        // 1. rootcmd 实例化
        // 2. 初始化各个命令
        init(argc, argv);
        // 3. 执行rootcmd
        rootcmd->Execute(argc, argv);
        return 0;
    } 
    catch (const std::exception& ex) {
        LOG_ERROR(std::string("fatal: ") + ex.what());
        std::cerr << "Fatal error: " << ex.what() << std::endl;
        return 1;
    } 
    catch (...) {
        LOG_ERROR("fatal: unknown error");
        std::cerr << "Fatal error: unknown error" << std::endl;
        return 1;
    }
    // rootcmd->Run();
}
