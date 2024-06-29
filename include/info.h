/**
 * @file info.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief info.h 声明与 info 命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef INFO_H
#define INFO_H
#include <string>
#include <vector>
#include "command.h"
#include "root.h"
using std::string;
using std::vector;

void infoCmd();     ///<info 命令Run操作的
void init_info();   ///<初始化 info 命令的内容
#endif