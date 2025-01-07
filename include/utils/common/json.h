#if !defined(UTILS_COMMON_JSON_H)
#define UTILS_COMMON_JSON_H
#include <boost/json.hpp>
#include <iostream>
#include <string>
#include <vector>
namespace json = boost::json;
// template <typename T>
// std::string marshal(const T& obj);

// template <typename T>
// T unmarshal(const std::string& jsonStr);
template <typename T>
std::string marshal(const T& obj) {
    // 将结构体转换为 JSON 字符串
    json::value jv = json::value_from(obj);
    return json::serialize(jv);
}

// 反序列化模板函数 string->class
template <typename T>
T unmarshal(const std::string& jsonStr) {
    json::value jv = json::parse(jsonStr);
    return json::value_to<T>(jv);
}
std::string format_json(const boost::json::value& jv, int indent = 0);

#endif // UTILS_COMMON_JSON_H)