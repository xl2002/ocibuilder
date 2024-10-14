#if !defined(BUILDIMAGE_BUILDIMAGE_H)
#define BUILDIMAGE_BUILDIMAGE_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <windows.h>

// 跨平台创建目录的函数
extern bool createDirectory(const std::string& path);

// 递归创建目录
extern bool createDirectories(const std::string& path);

// 为 repositories 中的镜像创建文件结构
extern void createImageStructure(const std::string& imageName, const std::string& basePath);

// 创建 OCI Registry 的主要文件结构
extern void createOCIRegistryStructure(const std::string& basePath);

// 创建指定文件
extern void createFile(const std::string& filePath, const std::string& content);

extern void createConf(const std::string& basePath);
// 创建 layout 文件和 index 文件
extern void createLayoutFiles(const std::string& basePath);

// 创建指定 SHA-256 哈希值的二进制文件
extern void createBinaryFile(const std::string& dirPath, const std::string& hashValue, const std::vector<unsigned char>& data);

void buildOCIImage();

#endif