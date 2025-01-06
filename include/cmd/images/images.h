/**
 * @file images.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief images.h 声明与 images 命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CMD_IMAGES_IMAGES_H
#define CMD_IMAGES_IMAGES_H
#include <string>
#include <vector>
#include "utils/cli/cobra/command.h"
#include "cmd/root/root.h"
using std::string;
using std::vector;
/**
 * @struct imagesOptions
 * @brief imagesOptions 代表 images 标志的结果
 * 
 */
struct imagesOptions
{
    /* data */
    bool    all=false;        ///<是否输出镜像的所有信息
};

void init_images();     
void imagesCmd(Command& cmd, vector<string> args,imagesOptions*iopts);     
#endif