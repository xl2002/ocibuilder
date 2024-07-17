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
#ifndef LOGIN_H
#define LOGIN_H
#include <string>
#include <vector>
#include "cobra/command.h"
#include "root.h"
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
void loginCmd();        
#endif