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
#if !defined(GO_STRING_H)
#define GO_STRING_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
bool hasPrefix(const std::string& str, const std::string& prefix);
std::string toLower(const std::string& str);
std::string vectorToString(const std::vector<std::string>& vec);
std::string Join(const std::vector<std::string>& vec, const std::string& delimiter);
std::string TrimSpace(const std::string& str);
std::string toUpper(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
bool cutPrefix(const std::string& str, const std::string& prefix, std::string& result);

#endif // GO_STRING_H
