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
#include "utils/cli/cobra/command.h"
#include "utils/cli/cli/common.h"
#include "cmd/root/root.h"
#include "utils/cli/cobra/args.h"
#include "utils/common/error.h"
#include "image/types/define/build.h"
#include <fstream>
#include <memory>
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
//#include <unistd.h>
#include "utils/cli/cli/build.h"
#include "image/types/reference/reference.h"
#include "cmd/build/imagebuildah/build.h"
#include "storage/storage/storage.h"
#include "utils/logger/ProcessSafeLogger.h"
// #include <filesystem>
// #include "context.h"
// #include "creat.h"
// using namespace std;

std::shared_ptr<Flagset> Getbuildflags(std::shared_ptr<BudResults> br);
std::shared_ptr<Flagset> GetLayerFlags(std::shared_ptr<LayerResults> lr);
std::shared_ptr<Flagset> GetFromAndBudFlags(std::shared_ptr<FromAndBudResults> fr);
std::shared_ptr<Flagset> GetUserNSFlags(std::shared_ptr<UserNSResults> ur);
void init_buildcmd();
void buildCmd(std::shared_ptr<Command> cmd, vector<string> args,std::shared_ptr<BuildOptions> iopts);
// string UsageTemplate();

void RemoveAll(const std::string& path);
#endif