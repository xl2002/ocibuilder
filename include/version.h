/**
 * @file version.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief version.h 声明与 version 命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef VERSION_H
#define VERSION_H
#include <string>
#include <vector>
#include "command.h"
#include "root.h"
using std::string;
using std::vector;
/**
 * @brief versionOptions 代表 version 标志的结果
 * 
 */
struct versionOptions
{
    /* data */
    bool    json=false;           ///<
};

void init_version();        ///<初始化 version 命令的内容
void versionCmd();          ///<version 命令Run操作的
#endif