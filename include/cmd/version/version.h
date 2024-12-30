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
    bool    json=false;           ///<
};
struct versionInfo{
    std::string Version;
    std::string CppVersion;
    std::string ImageSpec;
    // std::string RuntimeSpec;
    // std::string CniSpec;
    // std::string LibcniVersion;
    // std::string ImageVersion;
    // std::string GitCommit;
    std::string Built;
    std::string OsArch;
    std::string BuildPlatform;
    friend void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const versionInfo& vi){
        jv=boost::json::object{
            {"Version",vi.Version},
            {"C++ Version",vi.CppVersion},
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
void versionCmd(std::shared_ptr<versionOptions> iopts);
#endif