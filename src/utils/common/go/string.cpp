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

#ifdef __linux__
const char pathSeparator = '/';
#else
const char pathSeparator = '\\';
#endif

std::string FromSlash(const std::string& path) {
    std::string result = path;
#ifndef __linux__
    std::replace(result.begin(), result.end(), '/', pathSeparator);
#endif
    return result;
}

// string -> vector<uint8_t>
std::vector<uint8_t> stringToVector(const std::string& str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}

// vector<uint8_t> -> string
std::string vectorToString(const std::vector<uint8_t>& vec) {
    return std::string(vec.begin(), vec.end());
}
// 将时间点转换为 ISO 8601 格式字符串（带纳秒精度）
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