#include <iostream>
#include <fstream>
#include <string>
#include <boost/filesystem.hpp>  // 使用 Boost 文件系统
#include "image\buildah\tar_other.h"  // 包含 tarpp 的头文件

namespace fs = boost::filesystem;  // 使用 Boost 文件系统命名空间

int main() {
    // 创建一个输出文件流，指定输出路径为 E:\example.tar
    std::ofstream tar_file(".\\example.tar", std::ios::binary);
    if (!tar_file.is_open()) {
        std::cerr << "Failed to open tar file for writing." << std::endl;
        return 1;
    }

    // 创建一个 Tar 对象，传入输出文件流
    tarpp::Tar tar(tar_file);

    // 输入文件夹路径
    fs::path input_dir = ".\\tests";

    // 检查文件夹是否存在
    if (!fs::exists(input_dir) || !fs::is_directory(input_dir)) {
        std::cerr << "Input directory does not exist or is not a directory." << std::endl;
        return 1;
    }

    // 遍历文件夹中的所有文件和子文件夹
    for (fs::recursive_directory_iterator it(input_dir), end_it; it != end_it; ++it) {
        const fs::path& file_path = *it;

        // 如果是目录，添加目录条目到 tar 中
        if (fs::is_directory(file_path)) {
            std::string dir_name = file_path.string().substr(input_dir.string().size() + 1);  // 获取相对路径
            if (!dir_name.empty() && dir_name.back() != '/') {
                dir_name += '/';  // 确保目录以 '/' 结尾
            }

            // 使用 Boost 转换路径分隔符为正斜杠 '/'
            std::replace(dir_name.begin(), dir_name.end(), '\\', '/');

            // 添加目录到 tar 文件（使用默认 TarFileOptions）
            tar.add(dir_name, "");
            std::cout << "Added directory: " << dir_name << std::endl;
        }
        // 如果是普通文件，添加文件内容到 tar 中
        else if (fs::is_regular_file(file_path)) {
            std::ifstream file(file_path.string(), std::ios::binary);
            if (file.is_open()) {
                std::string content((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());

                // 获取相对路径作为 tar 中的文件名
                std::string tar_name = file_path.string().substr(input_dir.string().size() + 1);

                // 使用 Boost 转换路径分隔符为正斜杠 '/'
                std::replace(tar_name.begin(), tar_name.end(), '\\', '/');

                // 添加文件内容到 tar 中（使用默认 TarFileOptions）
                tar.add(tar_name, content);

                // 打印信息
                std::cout << "Added file: " << tar_name << std::endl;
            } else {
                std::cerr << "Failed to open file: " << file_path << std::endl;
            }
        }
    }

    // 完成打包
    tar.finalize();

    std::cout << "Tar file created successfully at E:\\example.tar!" << std::endl;

    return 0;
}