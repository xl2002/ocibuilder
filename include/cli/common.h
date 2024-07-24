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
#include "storage/storage.h"
#include <memory>
#include "cobra/command.h"
#include "define/types.h"
// #include "storage/runtime.h"
// #include "storage/image.h"
#include "storage/Utils.h"
#include "config/config.h"
using std::vector;
using std::string;
vector<string> SplitN(const string& str, const string& delimiter, size_t max_splits);
string ParseBool(string str);
shared_ptr<store> getStore(Command* cmd);
string GetFormat(string format);
bool UseLayers();
std::string JoinPath(const std::string& path1, const std::string& path2);
shared_ptr<DecryptConfig> DecryptConfigs(vector<string>decryptionKeys);
std::vector<std::string> LookupEnvVarReferences(const std::vector<std::string>& specs);
string Abspath(string path);

// class Context {
// public:
//     virtual ~Context() = default;
// };
// class emptyCtx : public Context {
// };
// std::shared_ptr<Context> TODO();
// std::shared_ptr<Context> getContext();

#endif