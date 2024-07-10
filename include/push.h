/**
 * @file push.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief push.h 声明与 push 命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef PUSH_H
#define PUSH_H
#include <string>
#include <vector>
#include "command.h"
#include "root.h"
using std::string;
using std::vector;
/**
 * @brief pushOptions 代表 push 标志的结果
 * 
 */
struct pushOptions
{
    /* data */
    string  format;         ///<
};

void init_push();
void pushCmd();
#endif