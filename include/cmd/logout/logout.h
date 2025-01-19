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
#ifndef CMD_LOGOUT_LOGOUT_H
#define CMD_LOGOUT_LOGOUT_H
#include <string>
#include <vector>
#include "utils/cli/cobra/command.h"
#include "cmd/root/root.h"
using std::string;
using std::vector;
/**
 * @class logoutOptions
 * @brief logoutOptions 代表 logout 标志的结果
 * 
 */
class logoutOptions
{
    public:
    /* data */
    bool    all=false;        ///<

};

void init_logout();     
void logoutCmd(Command& cmd, vector<string> args, logoutOptions* iopts);      

#endif