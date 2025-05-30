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
#ifndef CMD_PUSH_PUSH_H
#define CMD_PUSH_PUSH_H
#include <string>
#include <vector>
#include "utils/cli/cobra/command.h"
#include "cmd/root/root.h"
using std::string;
using std::vector;
/**
 * @brief pushOptions 代表 push 标志的结果
 * 
 */
class pushOptions
{
    public:
    string  format;         ///<
    std::string authfile;
};

void init_push();
void pushCmd(std::shared_ptr<Command> cmd, vector<string> args, std::shared_ptr<pushOptions>iopts);
void pushCmdLocal(std::shared_ptr<Command> cmd, vector<string> args, std::shared_ptr<pushOptions> iopts);
#endif