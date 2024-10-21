// #include <boost/filesystem.hpp>
// #include <iostream>
// int main()
// {
//     std::cout << boost::filesystem::exists("D:\\oci_images\\storage.conf") << std::endl;
// }

#include <iostream>
#include <regex>
#include <string>

int main() {
    std::string src = "# 使用 BusyBox 作为基镜像\n"
                      "这是有效代码\n"
                      "# 这是一条注释\n"
                      "另一行代码 # 这是行内注释\n";

    // 匹配以 # 开头的注释及其后面的所有内容
    static const std::regex tokenComment(R"(#.*?\n)", std::regex_constants::ECMAScript);

    // 输出原始字符串
    std::cout << "原始字符串:\n" << src << "\n\n";

    // 检查是否匹配
    std::smatch match;
    if (std::regex_search(src, match, tokenComment)) {
        std::cout << "找到匹配的注释: " << match[0] << "\n";
    } else {
        std::cout << "未找到匹配的注释。\n";
    }

    // 使用 regex_replace 清除注释
    std::string result = std::regex_replace(src, tokenComment, "");

    // 去除每行前后的空白字符
    result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)"), "");

    // 去除多余空行
    result = std::regex_replace(result, std::regex(R"(\n\s*\n)"), "\n");

    // 输出结果
    std::cout << "清除注释后的字符串:\n" << result << std::endl;

    return 0;
}



