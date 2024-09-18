#if !defined(FMT_PRINT_H)
#define FMT_PRINT_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <any>

// 辅助函数，用于将单个参数追加到字符串流中
template <typename T>
void appendToStream(std::ostringstream& oss, const T& arg);

// 递归模板函数，处理多个参数
template <typename T, typename... Args>
void appendToStream(std::ostringstream& oss, const T& first, const Args&... rest);

// 主函数，fmt.Sprintln
template <typename... Args>
std::string sprintlnn(const Args&... args);


#endif // FMT_PRINT_H