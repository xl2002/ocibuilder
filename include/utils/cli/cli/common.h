/**
 * @file common.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 声明通用方法和结构等等
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CLI_COMMON_H
#define CLI_COMMON_H
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <tuple>
#include "storage/storage/storage.h"
#include <memory>
#include "utils/cli/cobra/command.h"
#include "image/types/define/types.h"
// #include "storage/runtime.h"
// #include "storage/image.h"
#include "storage/types/options.h"
// #include "storage/utils.h"
#include "config/config.h"   //#include "cmd/config/config.h"
#include "storage/storage/storage_transport.h"
using std::vector;
using std::string;
vector<string> SplitN(const string& str, const string& delimiter, size_t max_splits);
string ParseBool(string str);
shared_ptr<Store> getStore(Command* cmd);
string GetFormat(string format);
bool UseLayers();
std::string JoinPath(const std::string& path1, const std::string& path2);
shared_ptr<DecryptConfig> DecryptConfigs(vector<string>decryptionKeys);
std::vector<std::string> LookupEnvVarReferences(const std::vector<std::string>& specs);
std::string Abspath(const std::string& path);
string UsageTemplate();
#endif