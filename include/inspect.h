/**
 * @file inspect.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief inspect.h 声明与 inspect 命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef INSPECT_H
#define INSPECT_H
#include <string>
#include <vector>
#include "command.h"
#include "root.h"
using std::string;
using std::vector;

void inspectCmd();      ///<inspect 命令Run操作的
void init_inspect();    ///<初始化 inspect 命令的内容
#endif