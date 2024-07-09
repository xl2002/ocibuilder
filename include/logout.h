/**
 * @file logout.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief logout.h 声明与 logout 命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LOGOUT_H
#define LOGOUT_H
#include <string>
#include <vector>
#include "command.h"
#include "root.h"
using std::string;
using std::vector;
/**
 * @struct logoutOptions
 * @brief logoutOptions 代表 logout 标志的结果
 * 
 */
struct logoutOptions
{
    /* data */
    bool    all=false;        ///<

};

void init_logout();     ///<初始化 logout 命令的内容
void logoutCmd();       ///<logout 命令Run操作的

#endif