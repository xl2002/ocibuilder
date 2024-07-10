/**
 * @file common.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对common.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "common.h"

/**
 * @brief 要围绕分隔符的第一个实例进行分割.
 * 
 * @param str 
 * @param delimiter 
 * @param max_splits 
 * @return vector<string> 
 */
vector<string> SplitN(const string& str, const string& delimiter, size_t max_splits) {
    vector<string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);
    size_t splits = 0;

    while (end != string::npos && splits < max_splits - 1) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
        ++splits;
    }
    result.push_back(str.substr(start)); // 添加字符串的剩余部分

    return result;
}
/**
 * @brief 将字符串解析成布尔类型
 * 
 * @param str 
 * @return string 
 */
string ParseBool(string str){
    string lower_str = str;
    transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);

    if (lower_str == "1" || lower_str == "t" || lower_str == "true") {
        return "true";
    }
    if (lower_str == "0" || lower_str == "f" || lower_str == "false") {
        return "false";
    }
    return "false";
}