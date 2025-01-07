#include <boost/filesystem.hpp>
#include <iostream>
#include <stdexcept>

namespace fs = boost::filesystem;

/**
 * @brief 复制目录及其内容到目标路径。
 * @param source 源目录路径。
 * @param destination 目标目录路径。
 * @return 成功复制的文件和目录数量。
 */
int64_t Copy_directory(fs::path& source, fs::path& destination) {
    // 检查源目录是否存在
    source=fs::absolute(source);
    if (!fs::exists(source) || !fs::is_directory(source)) {
        throw std::runtime_error("源目录不存在或不是一个目录: " + source.string());
    }

    // 检查目标路径是否存在，不存在则创建
    if (!fs::exists(destination)) {
        fs::create_directories(destination);
    } else if (!fs::is_directory(destination)) {
        throw std::runtime_error("目标路径已存在但不是一个目录: " + destination.string());
    }

    int64_t copy_count = 0; // 成功复制的文件和目录计数

    // 遍历源目录中的所有文件和子目录
    for (fs::recursive_directory_iterator it(source), end; it != end; ++it) {
        const fs::path& current = it->path();
        fs::path relative_path = fs::relative(current, source); // 计算相对路径
        fs::path target = destination / relative_path;          // 构造目标路径

        try {
            if (fs::is_directory(current)) {
                // 如果是目录，则创建对应目录
                fs::create_directory(target);
                ++copy_count;
            } else if (fs::is_regular_file(current)) {
                // 如果是文件，则复制到目标路径
                fs::copy_file(current, target, fs::copy_option::overwrite_if_exists);
                ++copy_count;
            } else {
                // 跳过符号链接或其他类型的文件
                std::cerr << "跳过不支持的文件类型: " << current << std::endl;
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "复制失败: " << current << " -> " << target << "\n原因: " << e.what() << std::endl;
        }
    }

    return copy_count;
}

// 示例用法
int main() {
    try {
        fs::path source = "baseimage";
        fs::path destination = "E:\\BaiduSyncdisk\\OCI\\oci_images\\overlay\\d874630577f501ab2e9884e45ea766ca159c3fabe8e3965017a0326ee7d9b5c6\\diff";

        int64_t count = Copy_directory(source, destination);
        std::cout << "成功复制了 " << count << " 个文件和目录。" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }

    return 0;
}
