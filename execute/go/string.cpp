#include "go/string.h"
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