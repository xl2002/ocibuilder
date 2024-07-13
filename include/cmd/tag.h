/**
 * @file tag.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief tag.h 声明与 tag 命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef TAG_H
#define TAG_H
#include <string>
#include <vector>
#include "cobra/command.h"
#include "root.h"
using std::string;
using std::vector;

void tagCmd();
void init_tag();
#endif