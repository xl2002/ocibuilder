/**
 * @file pull.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief pull.h 声明与 pull 命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CMD_PULL_PULL_H
#define CMD_PULL_PULL_H
#include <string>
#include <vector>
#include "utils/cli/cobra/command.h"
#include "cmd/root/root.h"
using std::string;
using std::vector;
/**
 * @brief pullOptions 代表 pull 标志的结果
 * 
 */
struct pullOptions
{
    /* data */
    bool    allTags=false;          ///<
    string  os;                     ///<
    string  arch;                   ///<
};
void pullCmd(Command& cmd, vector<string> args,pullOptions* iopts);
void init_pull();
#endif