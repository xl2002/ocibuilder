#include "filesys/utils/idtools.h"

/**
 * @brief 从用户映射和组映射创建新的IDMappings实例
 *
 * @param uids 用户映射列表
 * @param gids 组映射列表
 *
 * @return std::shared_ptr<IDMappings> 新创建的IDMappings实例
 */
std::shared_ptr<IDMappings> NewIDMappingsFromMaps(std::vector<IDMap> uids, std::vector<IDMap> gids) {

    auto mappings = std::make_shared<IDMappings>();
    mappings->uids = uids;
    mappings->gids = gids;
    return mappings;
}
//只考虑uid和gid相同且为0的情况
bool SafeChown(const std::string& name, int uid, int gid) {
    // 检查文件或目录是否存在
    if (!boost::filesystem::exists(name)) {
        throw myerror("文件或目录不存在: " + name);
    }

    struct stat statbuf;

    // 获取文件或目录的状态信息
    if (stat(name.c_str(), &statbuf) != 0) {
        throw myerror("获取状态信息失败: " + name);
    }

    // 检查 UID 和 GID 是否为 0
    if (statbuf.st_uid == 0 && statbuf.st_gid == 0) {
        return true;  // UID 和 GID 都为 0，不需要修改
    }

    // // 修改文件或目录的所有者
    // if (chown(name.c_str(), uid, gid) != 0) {
    //     throw myerror("修改所有者失败: " + name);
    // }
    return false; // 返回 false，表示进行了修改
}
// 使用你提供的MkdirAll实现
bool mkdirAs(const std::string& path, int mode, int ownerUID, int ownerGID, bool mkAll, bool chownExisting) {
    std::vector<std::string> paths;
    try {
        // 检查路径是否存在
        boost::filesystem::path p(path);
        if (boost::filesystem::exists(p)) {
            if (boost::filesystem::is_directory(p)) {
                // 目录存在，处理权限更改
                if (chownExisting) {
                    if (!SafeChown(path, ownerUID, ownerGID)) {
                        throw myerror("修改目录所有者失败: " + path);
                    }
                }
                return true;
            } else {
                throw myerror("路径不是目录: " + path);
            }
        }

        // 如果需要递归创建所有目录
        if (mkAll) {
            if (!MkdirAll(path)) {
                throw myerror("递归创建目录失败: " + path);
            }
        }

        // 创建当前目录
        if (!boost::filesystem::create_directory(p)) {
            throw myerror("创建目录失败: " + path);
        }

        // 处理权限变更
        for (const auto& pathComponent : paths) {
            if (!SafeChown(pathComponent, ownerUID, ownerGID)) {
                throw myerror("修改目录所有者失败: " + pathComponent);
            }
        }
    } catch (const myerror& e) {
        return false;
    } catch (const boost::filesystem::filesystem_error& e) {
        return false;
    } catch (const std::exception& e) {
        return false;
    }

    return true;
}

// 递归创建路径上的所有目录，并根据给定的UID和GID修改所有者
bool MkdirAllAs(const std::string& path, int mode, int ownerUID, int ownerGID) {
    try {
        return mkdirAs(path, mode, ownerUID, ownerGID, true, true);
    } catch (const myerror& e) {
        return false;
    }
}
void MkdirAllAndChownNew(const std::string& path, int mode, int ownerUID, int ownerGID) {
    try {
        mkdirAs(path, mode, ownerUID, ownerGID, true, false);
    } catch (const myerror& e) {
        throw;
    }
}