#if !defined(FILESYS_UTILS_IDTOOLS_H)
#define FILESYS_UTILS_IDTOOLS_H
#include <string>
#include <vector>
#include <memory>
#include "utils/common/error.h"
#include <boost/filesystem.hpp>
#include "storage/types/options.h"
class IDMap
{
    public:
    ~IDMap()=default;
    int ContainerID=0;
    int HostID=0;
    int Size=0;
};
// 定义 IDPair 类，用于表示 UID 和 GID 的配对
class IDPair {
    public:
    int UID=0; // 用户 ID
    int GID=0; // 组 ID
};
class IDMappings {
    public:
    std::vector<IDMap> uids;
    std::vector<IDMap> gids;
    // 构造函数
    IDMappings(std::vector<IDMap> uids, std::vector<IDMap> gids)
        : uids(std::move(uids)), gids(std::move(gids)) {}
    IDMappings() = default;
    std::vector<IDMap> UIDs(){
        return uids;
    }
    std::vector<IDMap> GIDs(){
        return gids;
    }
    bool empty(){
        return uids.empty() && gids.empty();
    }
};

std::shared_ptr<IDMappings> NewIDMappingsFromMaps(std::vector<IDMap> uids, std::vector<IDMap> gids);
bool MkdirAllAs(const std::string& path, int mode, int ownerUID, int ownerGID);
bool mkdirAs(const std::string& path, int mode, int ownerUID, int ownerGID, bool mkAll, bool chownExisting);
void MkdirAllAndChownNew(const std::string& path, int mode, int ownerUID, int ownerGID);



#endif // PKG_IDTOOLS_IDTOOLS_H
