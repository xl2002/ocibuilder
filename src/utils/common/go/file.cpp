#include "utils/common/go/file.h"
#include "utils/common/error.h"
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
bool isDirectoryWritable(const fs::path& dirPath) {
    try {
        if (!fs::exists(dirPath)) {
            std::cerr << "Directory does not exist: " << dirPath << std::endl;
            return false;
        }
        if (!fs::is_directory(dirPath)) {
            std::cerr << "Path is not a directory: " << dirPath << std::endl;
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
        std::cerr << "Error checking directory: " << e.what() << std::endl;
        return false;
    }
}
void makeDirectoryWritable(const fs::path& dirPath) {
    try {
        if (!fs::exists(dirPath)) {
            std::cerr << "Directory does not exist: " << dirPath << std::endl;
            return;
        }

        // 修改权限，添加写权限
        fs::permissions(dirPath, fs::perms::owner_write | fs::perms::add_perms);

        std::cout << "Permissions updated. Directory is now writable." << std::endl;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error changing permissions: " << e.what() << std::endl;
    }
}
/**
 * @brief 递归复制目录并统计数据传输总大小
 * @param source 源目录路径
 * @param destination 目标目录路径
 * @return int64_t 传输的数据总大小（字节）
 */
// int64_t Copy_directory(const fs::path& sourcedir, const fs::path& destination) {
//     // 检查源目录是否存在
//     std::chrono::duration<double> duration;
//     std::chrono::high_resolution_clock::time_point start,end;
//     start = std::chrono::high_resolution_clock::now();
//     int64_t total_size = 0; // 用于统计传输的数据大小
//     auto source=fs::absolute(sourcedir);
//     if (!fs::exists(source) || !fs::is_directory(source)) {
//         throw std::runtime_error("源目录不存在或不是一个目录: " + source.string());
//     }

//     // 检查目标路径是否存在，不存在则创建
//     if (!fs::exists(destination)) {
//         fs::create_directories(destination);
//     } else if (!fs::is_directory(destination)) {
//         throw std::runtime_error("目标路径已存在但不是一个目录: " + destination.string());
//     }

//     // int64_t copy_count = 0; // 成功复制的文件和目录计数

//     // 遍历源目录中的所有文件和子目录
//     for (fs::recursive_directory_iterator it(source), end; it != end; ++it) {
//         const fs::path& current = it->path();
//         fs::path relative_path = fs::relative(current, source); // 计算相对路径
//         fs::path target = destination / relative_path;          // 构造目标路径

//         try {
//             if (fs::is_directory(current)) {
//                 // 如果是目录，则创建对应目录
//                 fs::create_directory(target);
//                 // ++copy_count;
//             } else if (fs::is_regular_file(current)) {
//                 // 如果是文件，则复制到目标路径
//                 // 如果是文件，获取文件大小并复制
//                 int64_t file_size = fs::file_size(current);
//                 total_size += file_size; // 累加文件大小
//                 fs::copy_file(current, target, fs::copy_option::overwrite_if_exists);
//                 // ++copy_count;
//             } else {
//                 // 跳过符号链接或其他类型的文件
//                 std::cerr << "跳过不支持的文件类型: " << current << std::endl;
//             }
//         } catch (const fs::filesystem_error& e) {
//             std::cerr << "复制失败: " << current << " -> " << target << "\n原因: " << e.what() << std::endl;
//         }
//     }
//     end = std::chrono::high_resolution_clock::now();
//     duration = end - start;
//     std::cout<<"copy: "<<sourcedir.string()<<" -> "<<destination.string()<<" duration: "<<duration.count()<<" s"<<std::endl;
//     return total_size;
// }
// 复制单个文件的函数
void Copyfile(const fs::path& source, const fs::path& target, int64_t& total_size) {
    try {
        if (fs::is_regular_file(source)) {
            int64_t file_size = fs::file_size(source);
            total_size += file_size; // 累加文件大小
            fs::copy_file(source, target, fs::copy_option::overwrite_if_exists);
        } else {
            std::cerr << "跳过不支持的文件类型: " << source << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "复制失败: " << source << " -> " << target << "\n原因: " << e.what() << std::endl;
    }
}
// 复制目录的函数，使用多线程加速复制
int64_t Copy_directory(const fs::path& sourcedir, const fs::path& destination) {
    // std::chrono::duration<double> duration;
    // std::chrono::high_resolution_clock::time_point start,end;
    // start = std::chrono::high_resolution_clock::now();
    // 检查源目录是否存在
    int64_t total_size = 0; // 用于统计传输的数据大小
    fs::path source = fs::absolute(sourcedir);

    // 检查源目录是否存在
    if (!fs::exists(source) || !fs::is_directory(source)) {
        throw std::runtime_error("源目录不存在或不是一个目录: " + source.string());
    }

    // 检查目标路径是否存在，不存在则创建
    if (!fs::exists(destination)) {
        fs::create_directories(destination);
    } else if (!fs::is_directory(destination)) {
        throw std::runtime_error("目标路径已存在但不是一个目录: " + destination.string());
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
            std::cerr << "跳过不支持的文件类型: " << current << std::endl;
        }
    }

    // 使用 Boost 的线程池来并行处理文件复制
    boost::asio::thread_pool pool;

    // 并行创建目录
    for (const auto& dir : dirs_to_create) {
        boost::asio::post(pool, [dir]() {
            try {
                fs::create_directory(dir);
            } catch (const fs::filesystem_error& e) {
                std::cerr << "创建目录失败: " << dir << "\n原因: " << e.what() << std::endl;
            }
        });
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

    // end = std::chrono::high_resolution_clock::now();
    // duration = end - start;
    // std::cout<<"copy: "<<sourcedir.string()<<" -> "<<destination.string()<<" duration: "<<duration.count()<<" s"<<std::endl;
    return total_size;
}
bool IsPathSeparator(char c) {
    return c == Separator;
}

// 清理路径，确保路径规范化
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
bool isDirectoryEmpty(const boost::filesystem::path& dirPath) {
    try {
        if (boost::filesystem::exists(dirPath) && boost::filesystem::is_directory(dirPath)) {
            return boost::filesystem::directory_iterator(dirPath) == boost::filesystem::directory_iterator();
        } else {
            throw std::runtime_error("The path does not exist or is not a directory.");
        }
    } catch (const boost::filesystem::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false; // 如果出错，返回 false
    }
}
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
            std::cout << "Directory created: " << tempDirPath.string() << std::endl;
        } else {
            std::cerr << "Failed to create directory: " << tempDirPath.string() << std::endl;
        }
    } catch (const boost::filesystem::filesystem_error& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        throw;
    }

    // 返回创建的目录路径
    return tempDirPath.string();
}
namespace fs = boost::filesystem;

void Copy_file(const fs::path& src, const fs::path& dest) {
    try {
        // 检查源文件是否存在
        if (!fs::exists(src) || !fs::is_regular_file(src)) {
            throw std::runtime_error("Source file does not exist or is not a regular file: " + src.string());
        }

        // 检查目标文件的父目录是否存在，不存在则创建
        if (!fs::exists(dest.parent_path())) {
            fs::create_directories(dest.parent_path());
        }

        // 使用 Boost.Filesystem 的文件复制功能
        fs::copy_file(src, dest, fs::copy_option::overwrite_if_exists);
        // std::cout << "File copied successfully from " << src.string() << " to " << dest.string() << std::endl;
    } catch (const fs::filesystem_error& ex) {
        std::cerr << "Filesystem error: " << ex.what() << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}