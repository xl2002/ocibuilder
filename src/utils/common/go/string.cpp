/**
 * @file string.cpp
 * @brief 字符串处理工具函数集合
 * @details 提供各种字符串操作函数，包括大小写转换、前后缀处理、字符串分割等
 */
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
/**
 * @brief 检查一个字符串是否以另一个字符串结尾
 * 
 * @param s 要检查的字符串
 * @param suffix 后缀字符串
 * @return true 如果s以suffix结尾
 * @return false 如果s不以suffix结尾
 */
bool hasSuffix(const std::string& s, const std::string& suffix) {
    // 检查字符串长度，并比较后缀
    return s.size() >= suffix.size() && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}
/**
 * @brief 将字符串转换为小写
 * 
 * @param str 输入字符串
 * @return std::string 转换为小写后的字符串
 */
std::string toLower(const std::string& str) {
        std::string lower_str = str;
        std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
        return lower_str;
    }
/**
 * @brief 将字符串转换为大写
 * 
 * @param str 输入字符串
 * @return std::string 转换为大写后的字符串
 */
std::string toUpper(const std::string& str) {
    std::string result;
    result.resize(str.size());
    std::transform(str.begin(), str.end(), result.begin(), ::toupper);
    return result;
}

/**
 * @brief 将字符串向量转换为字符串表示
 * @details 主要用于错误消息显示
 * 
 * @param vec 字符串向量
 * @return std::string 格式为"[item1, item2, ...]"的字符串
 */
std::string vectorToString(const std::vector<std::string>& vec) {
    std::string result;
    if(vec.size() == 0) return "";
    for (const auto& item : vec) {
        if (!result.empty()) {
            result += ", ";
        }
        result += item;
    }
    return "[" + result + "]";
}
/**
 * @brief 使用分隔符连接字符串向量
 * 
 * @param vec 要连接的字符串向量
 * @param delimiter 分隔符
 * @return std::string 连接后的字符串
 */
std::string Join(const std::vector<std::string>& vec, const std::string& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i != 0) oss << delimiter;
        oss << vec[i];
    }
    return oss.str();
}
/**
 * @brief 去除字符串的后缀
 * @details 如果字符串以指定后缀结尾，则去除该后缀
 * 
 * @param str 输入字符串
 * @param suffix 要去除的后缀
 * @return std::string 去除后缀后的字符串(如果匹配)，否则返回原字符串
 */
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
/**
 * @brief 去除字符串的前缀
 * @details 如果字符串以指定前缀开头，则去除该前缀
 * 
 * @param s 输入字符串
 * @param prefix 要去除的前缀
 * @return std::string 去除前缀后的字符串(如果匹配)，否则返回原字符串
 */
std::string TrimPrefix(const std::string& s, const std::string& prefix) {
    if (s.find(prefix) == 0) {  // 检查 s 是否以 prefix 开头
        return s.substr(prefix.length());
    }
    return s;  // 如果没有前缀，返回原字符串
}
/**
 * @brief 去除字符串首尾的空白字符
 * @details 包括空格、制表符、换行符等
 * 
 * @param str 输入字符串
 * @return std::string 去除空白后的字符串
 */
std::string TrimSpace(const std::string& str) {
    const auto start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    const auto end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

/**
 * @brief 使用分隔符分割字符串
 * 
 * @param str 要分割的字符串
 * @param delimiter 分隔字符
 * @return std::vector<std::string> 分割后的字符串向量
 */
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

/**
 * @brief 去除字符串前缀并返回结果
 * @details 如果字符串以指定前缀开头，则去除前缀并将结果存入result
 * 
 * @param str 输入字符串
 * @param prefix 要去除的前缀
 * @param result 存储去除前缀后的结果
 * @return true 如果成功去除前缀
 * @return false 如果字符串不以指定前缀开头
 */
bool cutPrefix(const std::string& str, const std::string& prefix, std::string& result) {
    if (str.find(prefix) == 0) {
        result = str.substr(prefix.length());
        return true;
    }
    return false;
}
/**
 * @brief 在第一个分隔符处分割字符串
 * @details 返回分隔符前后的子字符串及是否找到分隔符的标志
 * 
 * @param str 输入字符串
 * @param delimiter 分隔字符
 * @return std::tuple<std::string, std::string, bool> 
 *         第一个元素是分隔符前的子字符串
 *         第二个元素是分隔符后的子字符串
 *         第三个元素表示是否找到分隔符
 */
std::tuple<std::string, std::string, bool> Cut(const std::string &str, char delimiter) {
    size_t pos = str.find(delimiter);
    if (pos == std::string::npos) {
        return std::make_tuple(str, "", false);
    }
    return std::make_tuple(str.substr(0, pos), str.substr(pos + 1), true);
}

#ifdef __linux__
const char pathSeparator = '/';
#else
const char pathSeparator = '\\';
#endif

/**
 * @brief 将路径中的斜杠转换为当前系统的路径分隔符
 * 
 * @param path 输入路径
 * @return std::string 转换后的路径
 */
std::string FromSlash(const std::string& path) {
    std::string result = path;
#ifndef __linux__
    std::replace(result.begin(), result.end(), '/', pathSeparator);
#endif
    return result;
}

// string -> vector<uint8_t>

/**
 * @brief 将字符串转换为字节向量
 * 
 * @param str 输入字符串
 * @return std::vector<uint8_t> 包含字符串字节的向量
 */
std::vector<uint8_t> stringToVector(const std::string& str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}

// vector<uint8_t> -> string
/**
 * @brief 将字节向量转换为字符串
 * 
 * @param vec 输入字节向量
 * @return std::string 转换后的字符串
 */
std::string vectorToString(const std::vector<uint8_t>& vec) {
    return std::string(vec.begin(), vec.end());
}
// 将时间点转换为 ISO 8601 格式字符串（带纳秒精度）
/**
 * @brief 将时间点转换为ISO 8601格式字符串
 * @details 格式为"YYYY-MM-DDTHH:MM:SS.ffffffZ"，带微秒精度
 * 
 * @param timePoint 要转换的时间点
 * @return std::string ISO 8601格式的时间字符串
 */
std::string timePointToISOString(const std::chrono::system_clock::time_point& timePoint) {

    // 获取秒级时间戳
    auto timeT = std::chrono::system_clock::to_time_t(timePoint);
    auto duration = timePoint - std::chrono::system_clock::from_time_t(timeT);
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

    // 转换为本地时间（UTC 时间）
    std::tm utcTime;
#ifndef __linux__
    gmtime_s(&utcTime, &timeT);
#else
    gmtime_r(&timeT, &utcTime);
#endif

    // 格式化秒级时间
    std::ostringstream oss;
    oss << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%S");

    // 添加微秒部分（补零确保6位数）
    oss << "." << std::setw(6) << std::setfill('0') << microseconds << "Z";

    return oss.str();
}
/**
 * @brief 将ISO 8601格式字符串解析为时间点
 * 
 * @param isoString ISO 8601格式的时间字符串
 * @return std::chrono::system_clock::time_point 解析后的时间点
 * @throws std::invalid_argument 如果输入字符串格式无效
 */
std::chrono::system_clock::time_point parseISOStringToTimePoint(const std::string& isoString) {
    // 定义时间点初值
    std::chrono::system_clock::time_point timePoint;

    // 去除末尾的 'Z'（如果存在）
    std::string trimmedIsoString = isoString;
    if (!isoString.empty() && isoString.back() == 'Z') {
        trimmedIsoString.pop_back();
    }

    // 分离日期和小数部分
    std::string dateTimePart;
    std::string fractionalPart = "0"; // 默认为 0 纳秒
    size_t dotPos = trimmedIsoString.find('.');
    if (dotPos != std::string::npos) {
        dateTimePart = trimmedIsoString.substr(0, dotPos);
        fractionalPart = trimmedIsoString.substr(dotPos + 1);
    } else {
        dateTimePart = trimmedIsoString;
    }

    // 解析日期和时间部分
    std::tm tm = {};
    std::istringstream dateTimeStream(dateTimePart);
    dateTimeStream >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    if (dateTimeStream.fail()) {
        throw std::invalid_argument("Invalid ISO 8601 date-time format: " + isoString);
    }

    // 转换为 time_t
    #ifndef __linux__
    std::time_t timeT =_mkgmtime(&tm);
    #else
    std::time_t timeT = timegm(&tm); // 将 UTC 时间转为 time_t
    #endif
    // 转换到 time_point
    timePoint = std::chrono::system_clock::from_time_t(timeT);

    // 处理小数部分（纳秒或微秒）
    if (!fractionalPart.empty()) {
        // 转换为纳秒单位
        int64_t fractionalNs = std::stoll(fractionalPart + std::string(9 - fractionalPart.length(), '0'));
        timePoint += std::chrono::nanoseconds(fractionalNs);
    }

    return timePoint;
}
