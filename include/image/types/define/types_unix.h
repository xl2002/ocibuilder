#if !defined(IMAGE_TYPES_DEFINE_TYPES_UNIX_H)
#define IMAGE_TYPES_DEFINE_TYPES_UNIX_H
#include <string>
#include <vector>
#include <map>
#include "filesys/system/devices.h"
class BuildahDevice:public Device{
    std::string Source;
    std::string Destination;
};
class ContainerDevices{
    public:
    std::vector<BuildahDevice> containerDevices;
};
#endif // DEFINE_TYPES_UNIX_H)
