#include "fmt/print.h"


// // 辅助函数：将单个参数添加到字符串流中
// template <typename T>
// void appendToStream(std::ostringstream& oss, const T& arg) {
//     oss << arg;
// }

// // 递归模板函数：处理多个参数，并在每个参数之间添加空格
// template <typename T, typename... Args>
// void appendToStream(std::ostringstream& oss, const T& first, const Args&... rest) {
//     oss << first << " ";            // 参数之间添加空格
//     appendToStream(oss, rest...);   // 递归处理剩余参数
// }

// // 主函数：将多个参数格式化为一个字符串，并去掉末尾多余的空格
// template <typename... Args>
// std::string sprintlnn(const Args&... args) {
//     std::ostringstream oss;
//     appendToStream(oss, args...);  // 调用递归函数处理所有参数
//     std::string result = oss.str();

//     // 如果结果字符串不为空且末尾存在空格，则删除末尾空格
//     if (!result.empty() && result.back() == ' ') {
//         result.pop_back();
//     }

//     return result;
// };