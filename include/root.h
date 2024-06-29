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
#ifndef ROOT_H
#define ROOT_H
#include <map>
#include "command.h"
// #include "flag.h"
// using namespace std;
/**
 * @brief 根命令的全局标志量
 * 
 */
struct globalFlags{
    bool Debug;                 ///<是否调试
    string Root;                ///<
    string RunRoot;             ///<
    string RegistriesConf;      ///<
    string RegistriesConfDir;   ///<
};
//

extern Command rootcmd; ///<声明全局根命令

Command init_rootcmd(); ///<rootcmd的初始化函数

#endif