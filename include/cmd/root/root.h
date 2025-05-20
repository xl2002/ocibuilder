/**
 * @file root.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 根命令的声明
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CMD_ROOT_ROOT_H
#define CMD_ROOT_ROOT_H
#include <map>
#include <memory>
#include "utils/cli/cobra/command.h"
#include "storage/types/options.h"
#include "config/new.h"
// #include "flag.h"
// using namespace std;
/**
 * @brief 根命令的全局标志量
 * 
 */
class globalFlags{
    public:
    bool Debug=false;                   ///<是否调试
    string Root;                        ///<
    // string RunRoot;                  ///<
    string RegistriesConf;              ///<
    string RegistriesConfDir;           ///<
    string DefaultMountsFile ;   
    string UserShortNameAliasConfPath;
    globalFlags()=default;
};
//
/**
 * @brief 全局变量
 * 
 */
extern std::shared_ptr <Command> rootcmd;                         ///<声明全局根命令
extern std::shared_ptr<Config> defaultContainerConfig;    ///<声明全局镜像config
extern int exitcode;                            ///<声明全局程序结束标志
extern globalFlags globalFlagOptions;           ///<声明全局标签选项



std::shared_ptr <Command> init_rootcmd();
void before(std::shared_ptr<Command> cmd);
void after(std::shared_ptr<Command> cmd);

#endif