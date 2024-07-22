#include "pkg/util/util.h"
#include <windows.h>
#include <sys/stat.h>
#include <vector>
/**
 * @brief 判断文件路径是否是描述符文件，如果是则将文件内容复制到临时文件中，并返回临时文件路径
 * 
 * @param file 要检查的文件路径
 * @param authfile 用于存储临时文件路径的引用
 * @return true 如果文件路径是描述符文件，并且成功复制到临时文件中
 * @return false 如果文件路径不是描述符文件，直接将文件路径存储到authfile中
 * @throws myerror 如果文件路径是描述符文件，但无法成功复制到临时文件中
 */
bool MirrorToTempFileIfPathIsDescriptor(std::string file ,std::string &authfile){
    // 如果文件路径不是以"/dev/fd/"开头，则认为不是描述符文件，直接返回false
    if(file.substr(0,8)!="/dev/fd/"){
        authfile=file;
        return false;
    }
    try
    {
        // 打开描述符文件
        std::ifstream infile(file,std::ios::binary);
        // 如果文件打开失败，则抛出异常
        if(!infile.is_open()){
            throw myerror("无法打开文件: " + file);
        }
        // 关闭文件
        infile.close();
    }
    catch(const myerror & e)
    {
        // 如果出现异常，则直接抛出异常
        throw;
    }
    // 如果文件路径是描述符文件，则将文件内容复制到临时文件中，并返回临时文件路径
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
 * @brief 查找容器文件
 * 
 * @param path 路径
 * @return std::string 容器文件路径
 * @throws myerror 无法找到容器文件或Dockerfile
 */
std::string DiscoverContainerfile(std::string path){
    // 定义目标文件结构体
    struct stat target;
    std::string foundCtrFile;

    // 使用stat函数检查文件是否存在，如果stat返回0，则文件存在
    if(stat(path.c_str(),&target)!=0){
        throw myerror("查找Containerfile: " + std::string(strerror(errno)));
    }
    // 如果是目录，查找Containerfile或Dockerfile
    if((target.st_mode& S_IFDIR)!=0){
        std::string ctrfile = joinPath(path, "Containerfile");
        // 如果Containerfile不存在，查找Dockerfile
        if (!fileExists(ctrfile)) {
            ctrfile = joinPath(path, "Dockerfile");
            if (!fileExists(ctrfile)) {
                throw myerror("在上下文目录中找不到Containerfile或Dockerfile");
            }
        }
        // 如果Containerfile是一个文件
        if (isRegularFile(ctrfile)) {
            foundCtrFile = ctrfile;
        } else {
            throw myerror("假定Containerfile " + ctrfile + " 不是一个文件");
        }
    } 
    // 如果是普通文件
    else if ((target.st_mode & S_IFREG) != 0) {
        foundCtrFile = path;
    } 
    // 如果不是文件也不是目录
    else {
        throw myerror("路径不是一个普通文件也不是目录");
    }
    return foundCtrFile;
}

/**
 * @brief 获取文件路径的目录部分
 * 
 * @param path 文件路径
 * @return std::string 该文件路径的目录部分
 * 
 * @throws None
 */
std::string getDirectory(const std::string& path) {
    // 使用find_last_of函数找到文件路径中的最后一个反斜杠或斜杠
    size_t pos = path.find_last_of("\\/"); // 使用反斜杠或斜杠作为分隔符
    if (pos == std::string::npos) { // 如果没有找到分隔符，返回空字符串
        return "";
    }
    // 返回文件路径的前 pos 个字符， 即文件路径的目录部分
    return path.substr(0, pos);
}
/**
 * @brief 解析符号链接
 * 
 * 该函数使用Win32 API GetFullPathNameA来解析符号链接，
 * 并将解析后的完整路径返回
 * 
 * @param path 需要解析的路径
 * @return std::string 解析后的完整路径
 * 
 * @throws std::runtime_error 如果解析符号链接失败
 */
std::string resolveSymlinks(const std::string& path) {
    // 申请一个MAX_PATH大小的缓冲区
    std::vector<char> buffer(MAX_PATH);
    // 使用GetFullPathNameA来解析符号链接
    DWORD result = GetFullPathNameA(path.c_str(), MAX_PATH, buffer.data(), nullptr);
    // 如果解析符号链接失败
    if (result == 0) {
        // 抛出一个std::runtime_error异常，并提供错误信息
        throw std::runtime_error("Error resolving symlinks: " + std::to_string(GetLastError()));
    }
    // 将解析后的完整路径返回
    return std::string(buffer.data());
}
