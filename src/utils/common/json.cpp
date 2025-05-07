#include "utils/common/json.h"

/**
 * @file json.cpp
 * @brief JSON格式化工具实现文件
 */

/**
 * @brief 格式化JSON值为可读字符串
 * @param jv 要格式化的JSON值
 * @param indent 初始缩进级别(空格数)
 * @return 格式化后的JSON字符串
 * @details 递归处理JSON对象和数组，生成带缩进的格式化字符串
 */
std::string format_json(const boost::json::value& jv, int indent) {
    std::string indent_str(indent, ' '); // 用于控制缩进
    std::ostringstream oss;

    if (jv.is_object()) {
        oss << "{\n";
        const auto& obj = jv.as_object();
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            oss << indent_str << "  \"" << it->key() << "\": ";
            oss << format_json(it->value(), indent + 2); // 递归处理对象
            if (std::next(it) != obj.end()) {
                oss << ",";
            }
            oss << "\n";
        }
        oss << indent_str << "}";
    } else if (jv.is_array()) {
        oss << "[\n";
        const auto& arr = jv.as_array();
        for (size_t i = 0; i < arr.size(); ++i) {
            oss << indent_str << "  " << format_json(arr[i], indent + 2);
            if (i + 1 < arr.size()) {
                oss << ",";
            }
            oss << "\n";
        }
        oss << indent_str << "]";
    } else if (jv.is_string()) {
        oss << jv.as_string();
    } else if (jv.is_double()) {
        oss << jv.as_double();
    } else if (jv.is_int64()) {
        oss << jv.as_int64();
    } else if (jv.is_bool()) {
        oss << (jv.as_bool() ? "true" : "false");
    } else {
        oss << "null";
    }

    return oss.str();
}
