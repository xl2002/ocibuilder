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
#ifndef COMMON_H
#define COMMON_H
#include <vector>
#include <string>
#include <algorithm>
using std::vector;
using std::string;
vector<string> SplitN(const string& str,const string& delimiter, size_t max_splits);
string ParseBool(string str);

#endif