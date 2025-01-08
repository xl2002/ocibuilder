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
#ifndef CMD_INSPECT_INSPECT_H
#define CMD_INSPECT_INSPECT_H
#include <string>
#include <vector>
#include "utils/cli/cobra/command.h"
#include "cmd/root/root.h"
using std::string;
using std::vector;

void inspectCmd(Command& cmd, vector<string> args);      
void init_inspect();    
#endif