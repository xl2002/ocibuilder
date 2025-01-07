#include <libtar/libtar.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <string>
namespace fs = boost::filesystem;

// 用来计算文件的大小并转成Octal格式
std::string toOctalString(size_t size, size_t width) {
    char buffer[width + 1];
    snprintf(buffer, sizeof(buffer), "%0*o", width, static_cast<unsigned int>(size));
    return std::string(buffer);
}

// 用来计算文件的大小并转成Octal格式
std::string toOctalString(size_t size, size_t width) {
    char buffer[width + 1];
    snprintf(buffer, sizeof(buffer), "%0*o", width, static_cast<unsigned int>(size));
    return std::string(buffer);
}

void createTar(const std::string& tarFilePath, const fs::path& directory) {
    // 打开tar文件以写入
    TAR* tar;
    // if (tar_open(&tar, tarFilePath.c_str(), NULL, O_WRONLY | O_CREAT, 0644, TAR_GNU) != 0) {
    //     std::cerr << "Failed to open tar file: " << tarFilePath << std::endl;
    //     return;
    // }

    // 遍历目录并处理其中的文件和子目录
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (fs::is_directory(entry)) {
            continue; // 忽略子目录，tar文件只包含文件
        }

        const fs::path& filePath = entry.path();
        
        // 获取相对于传入目录（directory）的路径
        std::string relativePath = fs::relative(filePath, directory).string();
        
        // 获取文件大小
        const size_t fileSize = fs::file_size(filePath);

        // 设置tar头信息
        struct stat fileStats;
        memset(&fileStats, 0, sizeof(fileStats));
        fileStats.st_mode = S_IFREG | 0644;  // 设置文件权限
        fileStats.st_size = fileSize;
        
        // 将文件添加到tar
        // if (tar_append_file(tar, relativePath.c_str(), &fileStats) != 0) {
        //     std::cerr << "Failed to append file: " << relativePath << std::endl;
        // }
    }

    // 关闭tar文件
    if (tar_close(tar) != 0) {
        std::cerr << "Failed to close tar file: " << tarFilePath << std::endl;
    }
}

int main() {
    // 指定tar文件路径和需要打包的目录
    const std::string tarFilePath = "./tests/output.tar";
    const fs::path directory = "./tests/182f7fe5f39439844fa081ef621851a9d5f4c6b6d5d1be9a008ece843c62eeb6";  // 需要打包的目录路径

    createTar(tarFilePath, directory);
    std::cout << "Tar file created successfully." << std::endl;

    return 0;
}