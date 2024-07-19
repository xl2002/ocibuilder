#if !defined(DEFINE_SPECS_H)
#define DEFINE_SPECS_H
#include <string>
using std::string;

struct LinuxIDMapping{
    uint32_t ContainerID;
    uint32_t HostID;
    uint32_t Size;
};

struct LinuxTimeOffset{
    int64_t Secs;
    uint32_t Nanosecs;
};

#endif // DEFINE_SPECS_H
