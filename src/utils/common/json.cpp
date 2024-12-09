#include "utils/common/json.h"
// 序列化模板函数 class->string
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