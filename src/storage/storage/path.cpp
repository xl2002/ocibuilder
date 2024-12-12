#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;


// // 判断字符是否为路径分隔符
// bool IsPathSeparator(char c) {
//     return c == Separator;
// }

// // 清理路径，确保路径规范化
// string Clean(const string& path) {
//     string originalPath = path;
//     string cleanPath = path;

//     // 处理卷标和根路径逻辑
//     size_t volLen = 0; // 根据需要定义卷标长度逻辑
//     if (cleanPath.empty()) {
//         if (volLen > 1 && IsPathSeparator(originalPath[0]) && IsPathSeparator(originalPath[1])) {
//             return originalPath; // 处理 UNC 路径
//         }
//         return originalPath + ".";
//     }

//     bool rooted = IsPathSeparator(cleanPath[0]);
//     size_t n = cleanPath.length();
//     stringstream out;
//     size_t r = 0, dotdot = 0;

//     if (rooted) {
//         out << Separator;
//         r = 1;
//         dotdot = 1;
//     }

//     while (r < n) {
//         if (IsPathSeparator(cleanPath[r])) {
//             // 跳过多余的分隔符
//             r++;
//         } else if (cleanPath[r] == '.' && (r + 1 == n || IsPathSeparator(cleanPath[r + 1]))) {
//             // 处理当前目录 '.'
//             r++;
//         } else if (cleanPath[r] == '.' && cleanPath[r + 1] == '.' && (r + 2 == n || IsPathSeparator(cleanPath[r + 2]))) {
//             // 处理上级目录 '..'
//             r += 2;
//             if (out.tellp() > dotdot) {
//                 // 可以回退
//                 stringstream temp;
//                 out.seekp(dotdot);
//                 while (out.tellp() > dotdot && !IsPathSeparator(out.str()[out.tellp() - 1])) {
//                     out.seekp(out.tellp() - 1);
//                 }
//                 out.str(temp.str());
//             } else if (!rooted) {
//                 // 不能回退且非根路径，添加 '..'
//                 if (out.tellp() > 0) {
//                     out << Separator;
//                 }
//                 out << "..";
//                 dotdot = out.tellp();
//             }
//         } else {
//             // 处理真实路径元素
//             if (rooted && out.tellp() != 1 || !rooted && out.tellp() != 0) {
//                 out << Separator;
//             }
//             for (; r < n && !IsPathSeparator(cleanPath[r]); r++) {
//                 out << cleanPath[r];
//             }
//         }
//     }

//     // 处理空路径情况
//     if (out.str().empty()) {
//         out << ".";
//     }

//     // 替换任何 `/` 为 Separator
//     string finalPath = out.str();
//     replace(finalPath.begin(), finalPath.end(), '/', Separator);
//     return finalPath;
// }

// // join 函数实现
// string join(const vector<string>& elem) {
//     // 遍历元素，找到第一个非空的元素作为起始点
//     auto it = find_if(elem.begin(), elem.end(), [](const string& e) { return !e.empty(); });
    
//     if (it != elem.end()) {
//         // 将剩下的元素用路径分隔符连接起来
//         stringstream joinedPath;
//         for (auto i = it; i != elem.end(); ++i) {
//             if (i != it) {
//                 joinedPath << Separator;
//             }
//             joinedPath << *i;
//         }
//         return Clean(joinedPath.str());
//     }
    
//     return "";
// }