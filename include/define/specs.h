#if !defined(DEFINE_SPECS_H)
#define DEFINE_SPECS_H
#include <string>
using std::string;
const string
	// PIDNamespace for isolating process IDs
	PIDNamespace  = "pid",
	// NetworkNamespace for isolating network devices, stacks, ports, etc
	NetworkNamespace  = "network",
	// MountNamespace for isolating mount points
	MountNamespace  = "mount",
	// IPCNamespace for isolating System V IPC, POSIX message queues
	IPCNamespace  = "ipc",
	// UTSNamespace for isolating hostname and NIS domain name
	UTSNamespace  = "uts",
	// UserNamespace for isolating user and group IDs
	UserNamespace  = "user",
	// CgroupNamespace for isolating cgroup hierarchies
	CgroupNamespace  = "cgroup",
	// TimeNamespace for isolating the clocks
	TimeNamespace  = "time";

struct LinuxIDMapping{
    uint32_t ContainerID;
    uint32_t HostID;
    uint32_t Size;
    LinuxIDMapping(uint32_t c,uint32_t h,uint32_t s):ContainerID(c),HostID(h),Size(s){};
};

struct LinuxTimeOffset{
    int64_t Secs;
    uint32_t Nanosecs;
};

#endif // DEFINE_SPECS_H
