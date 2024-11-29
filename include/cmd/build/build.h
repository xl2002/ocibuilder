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
#ifndef CMD_BUILD_BUILD_H
#define CMD_BUILD_BUILD_H
// #pragma once
#include "cobra/command.h"
#include "cli/common.h"
#include "root.h"
#include "cobra/args.h"
#include "cobra/error.h"
#include "image/types/define/build.h"
#include <fstream>
#include <memory>
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "cli/build.h"
#include "image/types/reference/reference.h"
#include "cmd/build/imagebuildah/build.h"
#include "storage/storage.h"
// #include <filesystem>
// #include "context.h"
// #include "creat.h"
// using namespace std;

Flagset* Getbuildflags(BudResults* br);
Flagset* GetLayerFlags(LayerResults* lr);
Flagset* GetFromAndBudFlags(FromAndBudResults* fr);
Flagset* GetUserNSFlags(UserNSResults* ur);
void init_buildcmd();
void buildCmd(Command& cmd, vector<string> args,BuildOptions* iopts);
string UsageTemplate();

void RemoveAll(const std::string& path);
#endif