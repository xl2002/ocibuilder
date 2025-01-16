/**
 * @file version.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief version.h 声明与 version 命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CMD_VERSION_VERSION_H
#define CMD_VERSION_VERSION_H
#include <string>
#include <vector>
#include "utils/cli/cobra/command.h"
#include "cmd/root/root.h"
#include <boost/json.hpp>
using std::string;
using std::vector;
/**
 * @brief versionOptions 代表 version 标志的结果
 * 
 */
struct versionOptions
{
    /* data */
    bool    json=false;           ///< json 标志
};
/**
 * @brief 存储版本信息及构建相关信息，包括C++版本、镜像规范等。
 * 
 */
struct versionInfo{
    std::string Version; //版本
    std::string CppVersion; //c++版本
    std::string ImageSpec;  //镜像规范
    // std::string RuntimeSpec;
    // std::string CniSpec;
    // std::string LibcniVersion;
    // std::string ImageVersion;
    // std::string GitCommit;
    std::string Built;  //编译时间
    std::string OsArch; //操作系统/架构
    std::string BuildPlatform; //工具构建的目标平台
    friend void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const versionInfo& vi){
        jv=boost::json::object{
            {"Version",vi.Version},
            {"Cpp Version",vi.CppVersion},
            {"Image Spec",vi.ImageSpec},
            // {"Runtime Spec",vi.RuntimeSpec},
            // {"Cni Spec",vi.CniSpec},
            // {"Libcni Version",vi.LibcniVersion},
            // {"Image Version",vi.ImageVersion},
            // {"Git Commit",vi.GitCommit},
            {"Built",vi.Built},
            {"Os/Arch",vi.OsArch},
            {"BuildPlatform",vi.BuildPlatform}
        };
    }
};
void init_version();
void versionCmd(versionOptions* iopts);
#endif