#include "pkg/util/util.h"
#include <windows.h>
#include <sys/stat.h>
#include <vector>
/**
 * @brief 如果给定的文件是指向Windows系统的文件描述符，
 *        则将其内容复制到一个临时文件中，并将临时文件的路径
 *        保存在authfile中
 * @param file  需要被复制的文件
 * @param authfile 保存临时文件的路径
 * @return 如果给定的文件是指向Windows系统的文件描述符，
 *         则返回true，否则返回false
 */
bool MirrorToTempFileIfPathIsDescriptor(std::string file ,std::string &authfile){
    if(file.substr(0,8)!="/dev/fd/"){
        authfile=file;
        return false;
    }
    try
    {
        std::ifstream infile(file,std::ios::binary);
        if(!infile.is_open()){
            throw myerror("Unable to open file: " + file);
        }
        infile.close();
    }
    catch(const myerror & e)
    {
        throw;
    }
    return true;
}

/**
 * @brief 将两个路径字符串连接在一起
 * 
 * @param path1 第一个路径字符串
 * @param path2 第二个路径字符串
 * @return 连接后的路径字符串
 */
std::string joinPath(const std::string& path1, const std::string& path2) {
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
 * @brief DiscoverContainerfile 该函数用来查找 Containerfile 文件
 *        如果给定的文件是一个目录，则查找该目录下的 Containerfile 文件
 *        如果给定的文件是一个普通文件，则直接返回该文件
 *        如果给定的文件既不是目录也不是普通文件
 *        则抛出异常
 * @param path 该参数是要查找的文件路径
 * @return std::string 如果查找成功，返回 Containerfile 的路径
 * @throws myerror 如果给定的文件既不是目录也不是普通文件
 */
std::string DiscoverContainerfile(std::string path){
    struct stat target;
    std::string foundCtrFile;
    if(stat(path.c_str(),&target)!=0){
        throw myerror("discovering Containerfile: " + std::string(strerror(errno)));
    }
    if((target.st_mode& S_IFDIR)!=0){
        std::string ctrfile = joinPath(path, "Containerfile");
        if (!fileExists(ctrfile)) {
            ctrfile = joinPath(path, "Dockerfile");
            if (!fileExists(ctrfile)) {
                throw myerror("cannot find Containerfile or Dockerfile in context directory");
            }
        }

        if (isRegularFile(ctrfile)) {
            foundCtrFile = ctrfile;
        } else {
            throw myerror("assumed Containerfile " + ctrfile + " is not a file");
        }
    } else if ((target.st_mode & S_IFREG) != 0) {
        foundCtrFile = path;
    } else {
        throw myerror("path is neither a regular file nor a directory");
    }
    return foundCtrFile;
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
 * @brief 解析符号链接
 * @param path 文件路径
 * @return 解析后的绝对路径
 */
std::string resolveSymlinks(const std::string& path) {
    std::vector<char> buffer(MAX_PATH);
    DWORD result = GetFullPathNameA(path.c_str(), MAX_PATH, buffer.data(), nullptr);
    if (result == 0) {
        throw std::runtime_error("Error resolving symlinks: " + std::to_string(GetLastError()));
    }
    return std::string(buffer.data());
}
