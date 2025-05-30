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
#ifndef CMD_TAG_TAG_H
#define CMD_TAG_TAG_H
#include <string>
#include <vector>
#include "utils/cli/cobra/command.h"
#include "utils/cli/cobra/args.h"
#include "cmd/root/root.h"
using std::string;
using std::vector;

void tagCmd(std::shared_ptr<Command> cmd, vector<string> args);
void init_tag();
#endif