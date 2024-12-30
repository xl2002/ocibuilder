/**
 * @file version.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief version.h 声明与 version 命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CMD_VERSION_VERSION_H
#define CMD_VERSION_VERSION_H
#include <string>
#include <vector>
#include "utils/cli/cobra/command.h"
#include "cmd/root/root.h"
using std::string;
using std::vector;
/**
 * @brief versionOptions 代表 version 标志的结果
 * 
 */
struct versionOptions
{
    /* data */
    bool    json=false;           ///<
};

void init_version();
void versionCmd(std::shared_ptr<versionOptions> iopts);
#endif