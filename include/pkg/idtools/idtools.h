#if !defined(PKG_IDTOOLS_IDTOOLS_H)
#define PKG_IDTOOLS_IDTOOLS_H
#include <string>
#include <vector>
#include <memory>
struct IDMap
{
    public:
    ~IDMap()=default;
    int ContainerID=0;
    int HostID=0;
    int Size=0;
};

struct IDMappings {
    std::vector<IDMap> uids;
    std::vector<IDMap> gids;
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


#endif // PKG_IDTOOLS_IDTOOLS_H
