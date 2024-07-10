/**
 * @file config.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief config.h声明与config命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <vector>
#include "command.h"
#include "root.h"
using std::string;
using std::vector;
/**
 * @brief  configOptions 代表config标志的结果
 * 
 */
struct configOptions
{
    /* data */
    bool    addHistory=false;   ///<向镜像中添加历史记录信息
    vector<string>  annotation; ///<更改镜像的annotation
    string  arch        ;       ///<更改镜像的 arch
    string  author      ;       ///<更改镜像的 author
    string  cmd         ;       ///<更改镜像的 cmd
    string  entrypoint  ;       ///<更改镜像的 entrypoint
    vector<string>  env ;       ///<更改镜像的 env
    vector<string> label;       ///<更改镜像的 label
    string  os          ;       ///<更改镜像的 os
};
void configCmd();   
void init_config(); 

#endif