/**
 * @file file.cpp
 * @brief 文件操作工具函数实现
 * @author 作者名
 * @date 2025-04-09
 */

#include "utils/common/go/file.h"
#include "utils/common/error.h"
#include "utils/logger/ProcessSafeLogger.h"
#include <boost/filesystem.hpp>
#include <random>
#include <string>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <sstream>
#include <iostream>
namespace fs = boost::filesystem;
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

    // 如果 path1 以分隔符结尾，则直接拼接
    if (path1.back() == Separator) {
        return path1 + path2;
    }

    // 否则在 path1 和 path2 之间插入分隔符
    return path1 + Separator + path2;
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
    return (stat(path.c_str(), &buffer) == 0);;
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
        logger->log_info("isDirectory: stat failed for path - " + path);
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
        logger->log_info("isRegularFile: stat failed for path - " + path);
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
/**
 * @brief 将内容写入指定文件
 * @param iidfile 要写入的文件路径
 * @param imageID 要写入的内容
 * @throws std::runtime_error 如果文件打开失败
 */
void WriteFile(const std::string& iidfile, const std::string& imageID) {
    try {
        // 创建文件并写入内容
        std::ofstream file(iidfile, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            LOG_ERROR("failed to open file - " + iidfile);
            throw std::runtime_error("failed to open file: " + iidfile);
        }

        // 写入 "sha256:" + imageID
        file << "sha256:" << imageID;
        if (!file) {
            LOG_ERROR("failed to write to file - " + iidfile);
            throw std::runtime_error("failed to write to file: " + iidfile);
        }
        file.close();

        // 返回成功的 imageID 和 ref
        // return std::make_tuple(imageID, ref, nullptr);
    } catch (...) {
        LOG_ERROR("exception occurred while writing to file - " + iidfile);
        throw;
        // 如果写入失败，返回捕获的异常
        // return std::make_tuple(imageID, ref, std::current_exception());
    }
}
/**
 * @brief 检查目录是否可写
 * @param dirPath 要检查的目录路径
 * @return true 目录存在且可写
 * @return false 目录不存在或不可写
 */
bool isDirectoryWritable(const fs::path& dirPath) {
    try {
        if (!fs::exists(dirPath)) {
            LOG_ERROR("isDirectoryWritable: directory does not exist - " + dirPath.string());
            // std::cerr << "Directory does not exist: " << dirPath << std::endl;
            return false;
        }
        if (!fs::is_directory(dirPath)) {
            LOG_ERROR("isDirectoryWritable: path is not a directory - " + dirPath.string());
            // std::cerr << "Path is not a directory: " << dirPath << std::endl;
            return false;
        }

        // 获取权限状态
        fs::file_status status = fs::status(dirPath);

        // 检查是否可写
        if ((status.permissions() & fs::perms::owner_write) != fs::perms::no_perms) {
            return true; // 可写
        } else {
            return false; // 不可写
        }
    } catch (const fs::filesystem_error& e) {
        LOG_ERROR("isDirectoryWritable: filesystem error - " + std::string(e.what()));
        // std::cerr << "Error checking directory: " << e.what() << std::endl;
        return false;
    }
}
/**
 * @brief 修改目录权限使其可写
 * @param dirPath 要修改的目录路径
 */
void makeDirectoryWritable(const fs::path& dirPath) {
    try {
        if (!fs::exists(dirPath)) {
            LOG_ERROR("makeDirectoryWritable: directory does not exist - " + dirPath.string());
            // std::cerr << "Directory does not exist: " << dirPath << std::endl;
            return;
        }

        // 修改权限，添加写权限
        fs::permissions(dirPath, fs::perms::owner_write | fs::perms::add_perms);

        std::cout << "Permissions updated. Directory is now writable." << std::endl;
    } catch (const fs::filesystem_error& e) {
        LOG_ERROR("makeDirectoryWritable: filesystem error - " + std::string(e.what()));
        // std::cerr << "Error changing permissions: " << e.what() << std::endl;
    }
}

// 跨平台复制文件
void Copyfile(const fs::path& source, const fs::path& target, int64_t& total_size) {
    try {
        if (fs::is_regular_file(source)) {
            int64_t file_size = fs::file_size(source);
            total_size += file_size; // 累加文件大小
            fs::copy_file(source, target, fs::copy_option::overwrite_if_exists);
        } else {
            LOG_ERROR("Copyfile: skip unsupported file types - " + source.string());
            // std::cerr << "Skip unsupported file types: " << source << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        LOG_ERROR("Copyfile: failed to copy file - " + source.string() + " -> " + target.string() + ", reason: " + e.what());
        // std::cerr << "复制失败: " << source << " -> " << target << "\n原因: " << e.what() << std::endl;
    }
}

// 复制目录，支持 Windows 和 Linux 系统
int64_t Copy_directory(const fs::path& sourcedir, const fs::path& destination) {
    int64_t total_size = 0; // 用于统计传输的数据大小
    fs::path source = fs::absolute(sourcedir);

    // 检查源目录是否存在
    if (!fs::exists(source) || !fs::is_directory(source)) {
        LOG_ERROR("source directory does not exist or is not a directory - " + source.string());
        throw std::runtime_error("The source directory does not exist or is not a directory: " + source.string());
    }

    // 检查目标路径是否存在，不存在则创建
    if (!fs::exists(destination)) {
        fs::create_directories(destination);
    } else if (!fs::is_directory(destination)) {
        LOG_ERROR("destination exists but is not a directory - " + destination.string());
        throw std::runtime_error("The destination path exists but is not a directory: " + destination.string());
    }

    // 用于存储所有文件和目录的路径
    std::vector<fs::path> files_to_copy;
    std::vector<fs::path> dirs_to_create;

    // 遍历源目录中的所有文件和子目录
    for (fs::recursive_directory_iterator it(source), end; it != end; ++it) {
        const fs::path& current = it->path();
        fs::path relative_path = fs::relative(current, source); // 计算相对路径
        fs::path target = destination / relative_path;          // 构造目标路径

        if (fs::is_directory(current)) {
            // 如果是目录，加入到 dirs_to_create
            dirs_to_create.push_back(target);
        } else if (fs::is_regular_file(current)) {
            // 如果是文件，加入到 files_to_copy
            files_to_copy.push_back(current);
        } else {
            LOG_ERROR("Copy_directory: skip unsupported file types - " + current.string());
            // std::cerr << "Skip unsupported file types: " << current << std::endl;
        }
    }

    // 使用 Boost 的线程池来并行处理文件复制
    boost::asio::thread_pool pool;

    // 创建目标目录部分，不使用多线程，直接顺序创建
    for (const auto& dir : dirs_to_create) {
        try {
            if (!fs::exists(dir)) {
                fs::create_directory(dir);
            }
        } catch (const fs::filesystem_error& e) {
            LOG_ERROR("Copy_directory: failed to create directory - " + dir.string() + ", reason: " + e.what());
            // std::cerr << "创建目录失败: " << dir << "\n原因: " << e.what() << std::endl;
        }
    }

    // 并行复制文件
    for (const auto& file : files_to_copy) {
        fs::path relative_path = fs::relative(file, source);
        fs::path target = destination / relative_path;
        
        boost::asio::post(pool, [file, target, &total_size]() {
            Copyfile(file, target, total_size);
        });
    }

    // 等待所有线程完成
    pool.join();

    return total_size;
}
/**
 * @brief 检查字符是否为路径分隔符
 * @param c 要检查的字符
 * @return true 字符是路径分隔符
 * @return false 字符不是路径分隔符
 */
bool IsPathSeparator(char c) {
    return c == Separator;
}

// 清理路径，确保路径规范化
/**
 * @brief 清理并规范化路径字符串
 * @param path 要清理的原始路径
 * @return 规范化后的路径字符串
 */
string Clean(const string& path) {
    string originalPath = path;
    string cleanPath = path;

    // 处理卷标和根路径逻辑
    size_t volLen = 0; // 根据需要定义卷标长度逻辑
    if (cleanPath.empty()) {
        if (volLen > 1 && IsPathSeparator(originalPath[0]) && IsPathSeparator(originalPath[1])) {
            return originalPath; // 处理 UNC 路径
        }
        return originalPath + ".";
    }

    bool rooted = IsPathSeparator(cleanPath[0]);
    size_t n = cleanPath.length();
    stringstream out;
    size_t r = 0, dotdot = 0;

    if (rooted) {
        out << Separator;
        r = 1;
        dotdot = 1;
    }

    while (r < n) {
        if (IsPathSeparator(cleanPath[r])) {
            // 跳过多余的分隔符
            r++;
        } else if (cleanPath[r] == '.' && (r + 1 == n || IsPathSeparator(cleanPath[r + 1]))) {
            // 处理当前目录 '.'
            r++;
        } else if (cleanPath[r] == '.' && cleanPath[r + 1] == '.' && (r + 2 == n || IsPathSeparator(cleanPath[r + 2]))) {
            // 处理上级目录 '..'
            r += 2;
            if (out.tellp() > dotdot) {
                // 可以回退
                stringstream temp;
                out.seekp(dotdot);
                while (out.tellp() > dotdot && !IsPathSeparator(out.str()[out.tellp() - 1])) {
                    out.seekp(out.tellp() - 1);
                }
                out.str(temp.str());
            } else if (!rooted) {
                // 不能回退且非根路径，添加 '..'
                if (out.tellp() > 0) {
                    out << Separator;
                }
                out << "..";
                dotdot = out.tellp();
            }
        } else {
            // 处理真实路径元素
            if (rooted && out.tellp() != 1 || !rooted && out.tellp() != 0) {
                out << Separator;
            }
            for (; r < n && !IsPathSeparator(cleanPath[r]); r++) {
                out << cleanPath[r];
            }
        }
    }

    // 处理空路径情况
    if (out.str().empty()) {
        out << ".";
    }

    // 替换任何 `/` 为 Separator
    string finalPath = out.str();
    replace(finalPath.begin(), finalPath.end(), '/', Separator);
    return finalPath;
}

// join 函数实现
/**
 * @brief 连接多个路径元素
 * @param elem 要连接的路径元素向量
 * @return 连接并规范化后的路径字符串
 */
string join(const vector<string>& elem) {
    // 遍历元素，找到第一个非空的元素作为起始点
    auto it = find_if(elem.begin(), elem.end(), [](const string& e) { return !e.empty(); });
    
    if (it != elem.end()) {
        // 将剩下的元素用路径分隔符连接起来
        stringstream joinedPath;
        for (auto i = it; i != elem.end(); ++i) {
            if (i != it) {
                joinedPath << Separator;
            }
            joinedPath << *i;
        }
        return Clean(joinedPath.str());
    }
    
    return "";
}
/**
 * @brief 检查目录是否为空
 * @param dirPath 要检查的目录路径
 * @return true 目录为空
 * @return false 目录不为空或检查失败
 * @throws std::runtime_error 如果路径不存在或不是目录
 */
bool isDirectoryEmpty(const boost::filesystem::path& dirPath) {
    try {
        if (boost::filesystem::exists(dirPath) && boost::filesystem::is_directory(dirPath)) {
            return boost::filesystem::directory_iterator(dirPath) == boost::filesystem::directory_iterator();
        } else {
            LOG_ERROR("path does not exist or is not a directory - " + dirPath.string());
            throw std::runtime_error("The path does not exist or is not a directory.");
        }
    } catch (const boost::filesystem::filesystem_error& e) {
        LOG_ERROR("Error: "+std::string(e.what()));
        // std::cerr << "Error: " << e.what() << std::endl;
        return false; // 如果出错，返回 false
    }
}
/**
 * @brief 创建临时目录
 * @param dir 临时目录的父目录
 * @param pattern 临时目录名前缀
 * @return 创建的临时目录完整路径
 * @throws boost::filesystem::filesystem_error 如果目录创建失败
 */
std::string MkdirTemp(std::string dir, std::string pattern) {
    // 确保 dir 路径以 '/' 或 '\' 结尾
    if (!dir.empty() && dir.back() != '/' && dir.back() != '\\') {
        dir += "/";
    }

    // 获取当前时间戳（秒）
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

    // 生成9位随机数，取时间戳的一部分
    int randomNumber = static_cast<int>(timestamp % 1000000000); // 截取时间戳的后9位

    // 确保9位随机数的范围在 100000000 到 999999999 之间
    if (randomNumber < 100000000) {
        randomNumber += 100000000;  // 如果随机数少于9位，确保它是9位
    }

    // 构造完整目录名
    std::ostringstream tempDirName;
    tempDirName << dir << pattern << randomNumber;

    // 转换为 boost::filesystem::path
    boost::filesystem::path tempDirPath(tempDirName.str());

    // 创建目录
    try {
        if (boost::filesystem::create_directories(tempDirPath)) {
            logger->log_info("Directory created: "+tempDirPath.string());
            std::cout << "Directory created: " << tempDirPath.string() << std::endl;
        } else {
            LOG_ERROR( "Failed to create directory: "+tempDirPath.string());
            // std::cerr << "Failed to create directory: " << tempDirPath.string() << std::endl;
        }
    } catch (const boost::filesystem::filesystem_error& e) {
        LOG_ERROR( "Error creating directory: "+std::string(e.what()));
        // std::cerr << "Error creating directory: " << e.what() << std::endl;
        throw;
    }

    // 返回创建的目录路径
    return tempDirPath.string();
}
namespace fs = boost::filesystem;

/**
 * @brief 复制单个文件
 * @param src 源文件路径
 * @param dest 目标文件路径
 * @throws std::runtime_error 如果源文件不存在或不是普通文件
 */
void Copy_file(const fs::path& src, const fs::path& dest) {
    try {
        // 检查源文件是否存在
        if (!fs::exists(src) || !fs::is_regular_file(src)) {
            LOG_ERROR("Copy_file: source file does not exist or is not regular - " + src.string());
            throw std::runtime_error("Source file does not exist or is not a regular file: " + src.string());
        }

        // 检查目标文件的父目录是否存在，不存在则创建
        if (!fs::exists(dest.parent_path())) {
            fs::create_directories(dest.parent_path());
        }

        // 使用 Boost.Filesystem 的文件复制功能
        fs::copy_file(src, dest, fs::copy_option::overwrite_if_exists);
        if (!fs::exists(dest) || !fs::is_regular_file(dest)) {
            LOG_ERROR("failed to copy file - " + src.string() + " to " + dest.string());
            throw std::runtime_error("Failed to copy file: " + src.string() + " to " + dest.string());
        }
        // std::cout << "File copied successfully from " << src.string() << " to " << dest.string() << std::endl;
    } catch (const fs::filesystem_error& ex) {
        LOG_ERROR("filesystem error - " + std::string(ex.what()));
        // std::cerr << "Filesystem error: " << ex.what() << std::endl;
    } catch (const std::exception& ex) {
        LOG_ERROR("error - " + std::string(ex.what()));
        // std::cerr << "Error: " << ex.what() << std::endl;
    }
}

/**
 * @brief 检查路径是否为绝对路径
 * @param path 要检查的路径字符串
 * @return true 路径是绝对路径
 * @return false 路径是相对路径
 */
bool isPathabsolute(const std::string& path) {
    return path[0] == '/' || path[1] == ':';
}
