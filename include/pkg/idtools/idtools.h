#if !defined(PKG_IDTOOLS_IDTOOLS_H)
#define PKG_IDTOOLS_IDTOOLS_H
#include <string>
#include <vector>
struct IDMap
{
    public:
    ~IDMap()=default;
    int ContainerID=0;
    int HostID=0;
};


#endif // PKG_IDTOOLS_IDTOOLS_H
