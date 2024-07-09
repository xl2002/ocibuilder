/**
 * @file build.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 该部分定义工具build命令相关的方法和变量
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef BUILD_H
#define BUILD_H
// #pragma once
#include "command.h"
#include "common.h"
#include "root.h"
#include "args.h"
// #include "context.h"
// #include "creat.h"

// using namespace std;
/**
 * @struct BuildOptions
 * @brief BuildOptions 代表build标志的结果
 * BuildOptions 中是build命令中合法flag的声明，
 * 后续在功能扩展时，只需在BuildOptions中添加即可添加flag
 */
struct BuildOptions{
    bool allplatform=false; ///<镜像构建是否适用所有平台
    vector<string> annotation; ///<向镜像元数据添加一个镜像annotation
    vector<string> tag; ///<镜像的标签
    vector<string> envs; ///<镜像的环境变量
    string osversion; ///<镜像支持的操作系统
};
/**
 * @struct LayerOptions
 * @brief LayerOptions 表示层标志的结果
 * 
 */
struct LayerOptions{
    bool ForceRm=false; ///<
    bool Layers=false; ///<
};

Flagset Getbuildflags(BuildOptions& br);///<返回通用构建标志
Flagset GetLayerFlags(LayerOptions& lr); ///<返回图层的通用标志
void init_buildcmd();    ///< 初始化build命令的内容
void buildCmd(Command& cmd, vector<string>& args);         ///< build 命令Run操作的
string UsageTemplate();
#endif