#include "utils/common/go/string.h"
#include <algorithm>
#include <sstream>
/**
 * @brief 检查一个字符串是否以另一个字符串开始
 *
 * @param str 字符串
 * @param prefix 前缀
 * @return true 如果 str 以 prefix 开始，则返回 true；否则返回 false
 */
bool hasPrefix(const std::string& str, const std::string& prefix) {
    return str.compare(0, prefix.length(), prefix) == 0;
}
// 判断字符串 s 是否以 suffix 结尾的函数
bool hasSuffix(const std::string& s, const std::string& suffix) {
    // 检查字符串长度，并比较后缀
    return s.size() >= suffix.size() && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}
std::string toLower(const std::string& str) {
        std::string lower_str = str;
        std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
        return lower_str;
    }
// 转换为大写
std::string toUpper(const std::string& str) {
    std::string result;
    result.resize(str.size());
    std::transform(str.begin(), str.end(), result.begin(), ::toupper);
    return result;
}

// Helper function to convert vector to string for error message
std::string vectorToString(const std::vector<std::string>& vec) {
    std::string result;
    for (const auto& item : vec) {
        if (!result.empty()) {
            result += ", ";
        }
        result += item;
    }
    return "[" + result + "]";
}
// Helper function to join a vector of strings with a delimiter
std::string Join(const std::vector<std::string>& vec, const std::string& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i != 0) oss << delimiter;
        oss << vec[i];
    }
    return oss.str();
}
// 实现 strings.TrimSuffix
std::string TrimSuffix(const std::string& str, const std::string& suffix) {
    if (suffix.empty() || str.size() < suffix.size()) {
        // 如果后缀为空，或者字符串长度小于后缀长度，直接返回原字符串
        return str;
    }

    // 检查字符串是否以指定的后缀结尾
    if (str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0) {
        // 去除后缀
        return str.substr(0, str.size() - suffix.size());
    }

    // 如果字符串不以指定后缀结尾，返回原字符串
    return str;
}
// TrimPrefix 返回去掉前缀字符串后的 s。
// 如果 s 没有以 prefix 开头，返回 s 不变。
std::string TrimPrefix(const std::string& s, const std::string& prefix) {
    if (s.find(prefix) == 0) {  // 检查 s 是否以 prefix 开头
        return s.substr(prefix.length());
    }
    return s;  // 如果没有前缀，返回原字符串
}
// Helper function to trim leading and trailing whitespace
std::string TrimSpace(const std::string& str) {
    const auto start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    const auto end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool cutPrefix(const std::string& str, const std::string& prefix, std::string& result) {
    if (str.find(prefix) == 0) {
        result = str.substr(prefix.length());
        return true;
    }
    return false;
}
std::tuple<std::string, std::string, bool> Cut(const std::string &str, char delimiter) {
    size_t pos = str.find(delimiter);
    if (pos == std::string::npos) {
        return std::make_tuple(str, "", false);
    }
    return std::make_tuple(str.substr(0, pos), str.substr(pos + 1), true);
}

#ifdef _WIN32
const char pathSeparator = '\\';
#else
const char pathSeparator = '/';
#endif

std::string FromSlash(const std::string& path) {
    std::string result = path;
#ifdef _WIN32
    std::replace(result.begin(), result.end(), '/', pathSeparator);
#endif
    return result;
}
