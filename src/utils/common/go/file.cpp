#include "utils/common/go/file.h"
#include "utils/common/error.h"
/**
 * @brief 将两个路径字符串连接在一起
 * 
 * @param path1 第一个路径字符串
 * @param path2 第二个路径字符串
 * @return 连接后的路径字符串
 */
std::string joinPath(const std::string& path1, const std::string& path2) {
    if (path1.empty()) {
        return path2;
    }
    if (path2.empty()) {
        return path1;
    }
    // 如果path1以反斜杠结尾，则直接拼接
    if (path1.back() == '\\') {
        return path1 + path2;
    }
    // 否则在path1和path2之间插入一个反斜杠
    return path1 + "\\" + path2;
}

/**
 * @brief 检查文件是否存在
 * 
 * @param path 文件路径
 * @return true 文件存在
 * @return false 文件不存在
 */
bool fileExists(const std::string& path) {
    // 定义一个结构体，用于存储文件信息
    struct stat buffer;
    // 使用stat函数检查文件是否存在，如果stat返回0，则文件存在
    // 否则，文件不存在
    return (stat(path.c_str(), &buffer) == 0);
}

/**
 * @brief 检查文件是否是一个目录
 * 
 * @param path 文件路径
 * @return true 文件是一个目录
 * @return false 文件不是一个目录
 */
bool isDirectory(const std::string& path) {
    struct stat buffer;
    // 使用stat函数检查文件是否存在，如果stat返回0，则文件存在
    // 并且检查文件是否是一个目录
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }
    // st_mode 是 stat 结构体中的一个成员变量，记录了文件的类型
    // S_IFDIR 是一个宏常量，表示该文件是一个目录
    return (buffer.st_mode & S_IFDIR) != 0;
}

/**
 * @brief 检查文件是否是一个普通文件
 * 
 * @param path 文件路径
 * @return true 文件是一个普通文件
 * @return false 文件不是一个普通文件
 */
bool isRegularFile(const std::string& path) {
    struct stat buffer;
    // 使用stat函数检查文件是否存在，如果stat返回0，则文件存在
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }
    // st_mode 是 stat 结构体中的一个成员变量，记录了文件的类型
    // S_IFREG 是一个宏常量，表示该文件是一个普通文件
    return (buffer.st_mode & S_IFREG) != 0;
}

/**
 * @brief 获取文件路径的目录部分
 * @param path 文件路径
 * @return 目录部分
 */
std::string getDirectory(const std::string& path) {
    size_t pos = path.find_last_of("\\/");
    if (pos == std::string::npos) {
        return "";
    }
    return path.substr(0, pos);
}
void  WriteFile(const std::string& iidfile, const std::string& imageID) {
    try {
        // 创建文件并写入内容
        std::ofstream file(iidfile, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + iidfile);
        }

        // 写入 "sha256:" + imageID
        file << "sha256:" << imageID;
        file.close();

        // 返回成功的 imageID 和 ref
        // return std::make_tuple(imageID, ref, nullptr);
    } catch (...) {
        throw;
        // 如果写入失败，返回捕获的异常
        // return std::make_tuple(imageID, ref, std::current_exception());
    }
}
/**
 * @brief 递归复制目录并统计数据传输总大小
 * @param source 源目录路径
 * @param destination 目标目录路径
 * @return int64_t 传输的数据总大小（字节）
 */
int64_t Copy_directory(const fs::path& source, const fs::path& destination) {
    int64_t total_size = 0; // 用于统计传输的数据大小

    try {
        // 检查源路径是否存在且是目录
        if (!fs::exists(source) || !fs::is_directory(source)) {
            throw std::runtime_error("Source directory does not exist or is not a directory.");
        }

        // 如果目标目录不存在，创建它
        if (!fs::exists(destination)) {
            fs::create_directories(destination);
        }

        // 遍历源目录内容
        for (const auto& entry : fs::directory_iterator(source)) {
            const fs::path& sourcePath = entry.path();
            fs::path destinationPath = destination / sourcePath.filename();

            if (fs::is_directory(sourcePath)) {
                // 如果是子目录，递归调用
                total_size += Copy_directory(sourcePath, destinationPath);
            } else if (fs::is_regular_file(sourcePath)) {
                // 如果是文件，获取文件大小并复制
                int64_t file_size = fs::file_size(sourcePath);
                total_size += file_size; // 累加文件大小
                fs::copy_file(sourcePath, destinationPath, fs::copy_option::overwrite_if_exists);
            } else {
                std::cerr << "Skipping unsupported file type: " << sourcePath.string() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error copying directory: " << e.what() << std::endl;
    }

    return total_size; // 返回总传输大小
}
