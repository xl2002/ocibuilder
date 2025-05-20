/**
 * @file save.h
 * @author zhaoqinlin
 * @brief save.h 声明与 save 命令相关的结构和方法 修改自push.h
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CMD_SAVE_SAVE_H
#define CMD_SAVE_SAVE_H
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
class saveOptions
{
    public://TODO: 三条参数
    //std::string authfile;  // 认证文件路径（可选）
    std::string output;    // 输出文件路径（必选）
    std::string image;     // 镜像名称（必选）
};

void init_save();
void saveCmd(std::shared_ptr<Command> cmd, vector<string> args, std::shared_ptr<saveOptions> iopts);
bool convertOciConfigToDockerLayerConfig(const std::string &ociConfigPath, const std::string &dockerConfigPath, const std::string &configDigest);
#endif