#if !defined(IMAGEBUILDER_INTERNALS_H)
#define IMAGEBUILDER_INTERNALS_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <functional>
#include <tuple>
#include <stdexcept>
#include <chrono>
// #include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include <limits.h>
#ifdef _WIN32
    #include <winsock2.h>
#include <windows.h>
    const char PATH_SEPARATOR = '\\';
#else
    #include <unistd.h>
    const char PATH_SEPARATOR = '/';
#endif
#include <cstdlib>
#include "go/string.h"
#include <boost/filesystem.hpp>
std::vector<std::string> mergeEnv(std::vector<std::string> defaults, std::vector<std::string> overrides);
// envMapAsSlice 返回一个包含 "key=value" 格式字符串的 vector
std::vector<std::string> envMapAsSlice(const std::map<std::string, std::string>& m);

// 根据平台返回特定的错误信息
std::string platformSupports(const std::string& command);

bool hasEnvName(std::vector<std::string> env, std::string name);
// 处理 JSON 参数的函数
std::vector<std::string> handleJSONArgs(const std::vector<std::string>& args, const std::map<std::string, bool>& attributes);
bool hasSlash(std::string input);
// 确保提供的路径是绝对路径
std::string makeAbsolute(const std::string& dest, const std::string& workingDir);

// 解析选项间隔时间，返回其持续时间，如果为空则返回 0。如果值是负数，则报告错误。
std::chrono::milliseconds parseOptInterval(const std::string& flagValue, const std::string& flagName);
#endif // IMAGEBUILDER_INTERNALS_H
