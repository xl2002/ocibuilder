/**
 * @file rmi.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief rmi.h 声明与 rmi 命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef RMI_H
#define RMI_H
#include <string>
#include <vector>
#include "command.h"
#include "root.h"
using std::string;
using std::vector;

void rmiCmd();      ///<rmi 命令Run操作的
void init_rmi();    ///<初始化 rmi 命令的内容
#endif