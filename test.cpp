// #include <boost/filesystem.hpp>
// #include <iostream>
// int main()
// {
//     std::cout << boost::filesystem::exists("D:\\oci_images\\storage.conf") << std::endl;
// }

// #include <iostream>
// #include <regex>
// #include <string>

// int main() {
//     std::string src = "# 使用 BusyBox 作为基镜像\n"
//                       "这是有效代码\n"
//                       "# 这是一条注释\n"
//                       "另一行代码 # 这是行内注释\n";

//     // 匹配以 # 开头的注释及其后面的所有内容
//     static const std::regex tokenComment(R"(#.*?\n)", std::regex_constants::ECMAScript);

//     // 输出原始字符串
//     std::cout << "原始字符串:\n" << src << "\n\n";

//     // 检查是否匹配
//     std::smatch match;
//     if (std::regex_search(src, match, tokenComment)) {
//         std::cout << "找到匹配的注释: " << match[0] << "\n";
//     } else {
//         std::cout << "未找到匹配的注释。\n";
//     }

//     // 使用 regex_replace 清除注释
//     std::string result = std::regex_replace(src, tokenComment, "");

//     // 去除每行前后的空白字符
//     result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)"), "");

//     // 去除多余空行
//     result = std::regex_replace(result, std::regex(R"(\n\s*\n)"), "\n");

//     // 输出结果
//     std::cout << "清除注释后的字符串:\n" << result << std::endl;

//     return 0;
// }
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <map>
#include <string>

// 遍历ptree并将数据存储到map
void traverse_ptree_to_map(const boost::property_tree::ptree& pt, std::map<std::string, std::string>& result_map, const std::string& path = "") {
    for (const auto& kv : pt) {
        std::string full_path = path.empty() ? kv.first : path + "." + kv.first;

        if (kv.second.empty()) {
            // 如果没有子节点，将 key 和 value 存入 map
            result_map[full_path] = kv.second.get_value<std::string>();
        } else {
            // 如果有子节点，递归遍历
            traverse_ptree_to_map(kv.second, result_map, full_path);
        }
    }
}

int main() {
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini("D:\\oci_images\\short-name-aliases.conf", pt);
    std::map<std::string, std::string> config_map;
    // 获取配置信息
    // 遍历并将配置文件的内容存储到map
    traverse_ptree_to_map(pt, config_map);

    // 输出 map 中的数据
    for (const auto& entry : config_map) {
        std::cout << entry.first << " = " << entry.second << std::endl;
    }

    return 0;
}


