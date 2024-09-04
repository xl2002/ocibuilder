#if !defined(DEFINE_TYPES_UNIX_H)
#define DEFINE_TYPES_UNIX_H
#include <string>
#include <vector>
#include <map>
#include "devices/devices.h"
class BuildahDevice:public Device{
    std::string Source;
    std::string Destination;
};
class ContainerDevices{
    public:
    std::vector<BuildahDevice> containerDevices;
};
#endif // DEFINE_TYPES_UNIX_H)
