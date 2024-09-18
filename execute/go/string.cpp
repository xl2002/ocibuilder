#include "go/string.h"

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

//data是否以suffix结尾
bool hasSuffix(const std::string& data, const std::string& suffix)
{
    return data.size() >= suffix.size() && data.compare(data.size() - suffix.size(), suffix.size(), suffix) == 0;
}
