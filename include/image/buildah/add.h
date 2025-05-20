#if !defined(CMD_BUILD_ADD_H)
#define CMD_BUILD_ADD_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <iosfwd>
#include "image/types/define/types.h"

// AddAndCopyOptions 类表示 add 和 copy 命令的选项
class AddAndCopyOptions {
public:
    std::string Chmod;                        // 设置目标内容的访问权限
    std::string Chown;                        // 设置新添加内容的所有权
    std::string Checksum;                     // 预期的内容哈希值（算法:摘要）
    bool PreserveOwnership = false;           // 如果 Chown 未设置，则保留现有的所有权
    std::shared_ptr<ostream> Hasher = nullptr;           // 数据哈希输出流（内容将传递给此流）
    std::vector<std::string> Excludes;        // .containerignore 文件的内容
    std::string IgnoreFile;                   // .containerignore 文件的路径
    std::string ContextDir;                   // 要复制内容的基本目录
    std::shared_ptr<::IDMappingOptions> IDMappingOptions=std::make_shared<::IDMappingOptions>();   // ID 映射选项（使用 shared_ptr 管理）
    bool DryRun = false;                      // 仅计算摘要而不复制内容
    bool StripSetuidBit = false;              // 清除 setuid 位
    bool StripSetgidBit = false;              // 清除 setgid 位
    bool StripStickyBit = false;              // 清除 sticky 位

    // 默认构造函数
    AddAndCopyOptions() = default;

    // 析构函数
    ~AddAndCopyOptions() = default;
};
#endif // CMD_BUILD_ADD_H
