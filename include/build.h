/**
 * @file build.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 该部分定义工具build命令相关的方法和变量
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef BUILD_H
#define BUILD_H
// #pragma once
#include "command.h"
#include "common.h"
#include "root.h"
// #include "context.h"
// #include "creat.h"

// using namespace std;

void init_buildcmd();   ///< 初始化build命令的内容
void buildCmd();         ///< build 命令Run操作的

#endif