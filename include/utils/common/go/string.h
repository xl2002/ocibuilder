/**
 * @file string.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 这个文件用于实现go语言内置的函数对应的C++代码
 * @version 0.1
 * @date 2024-07-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#if !defined(UTILS_COMMON_GO_STRING_H)
#define UTILS_COMMON_GO_STRING_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <iostream>
// #include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <time.h>
bool hasPrefix(const std::string& str, const std::string& prefix);
bool hasSuffix(const std::string& s, const std::string& suffix);
std::string toLower(const std::string& str);
std::string vectorToString(const std::vector<std::string>& vec);
std::string Join(const std::vector<std::string>& vec, const std::string& delimiter);
std::string TrimSuffix(const std::string& str, const std::string& suffix);
std::string TrimPrefix(const std::string& s, const std::string& prefix);
std::string TrimSpace(const std::string& str);
std::string toUpper(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
bool cutPrefix(const std::string& str, const std::string& prefix, std::string& result);
std::tuple<std::string, std::string, bool> Cut(const std::string &str, char delimiter);
std::string FromSlash(const std::string& path);
std::vector<uint8_t> stringToVector(const std::string& str);
std::string vectorToString(const std::vector<uint8_t>& vec);
std::string timePointToISOString(const std::chrono::system_clock::time_point& timePoint);
std::chrono::system_clock::time_point parseISOStringToTimePoint(const std::string& isoString);
#endif // GO_STRING_H
