/**
 * @file login.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief login.h 声明与 login 命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CMD_LOGIN_LOGIN_H
#define CMD_LOGIN_LOGIN_H
#include <string>
#include <vector>
#include "utils/cli/cobra/command.h"
#include "cmd/root/root.h"
using std::string;
using std::vector;
/**
 * @brief LoginOptions 代表 login 标志的结果
 * 
 */
struct LoginOptions
{
    /* data */
    string  username;
    string  password;
    bool    getLogin=false;
};

void init_login();      
void loginCmd(Command& cmd, vector<string> args,std::shared_ptr<LoginOptions> iopts);      
#endif