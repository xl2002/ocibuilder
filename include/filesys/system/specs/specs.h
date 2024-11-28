#if !defined(FILESYS_SYSTEM_SPECS_SPECS_H)
#define FILESYS_SYSTEM_SPECS_SPECS_H
#include <string>
#include <vector>
using std::string;
using std::vector;
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

/**
 * @brief LinuxTimeOffset 保存时间偏移
 * LinuxTimeOffset 结构体保存了时间偏移，包括以下字段:
 * - Secs: 秒偏移
 * - Nanosecs: 纳秒偏移
 */
struct LinuxTimeOffset{
    int64_t Secs;
    uint32_t Nanosecs;
};
/**
 * @brief Mount 表示一个挂载点
 * 
 * Mount 结构定义了一个挂载点，包括以下字段：
 * - Destination: 挂载点的目标路径
 * - Type: 挂载点的类型
 * - Source: 挂载点的来源路径
 * - Options: 挂载点的选项
 * - UIDMappings: 用户 ID 映射列表
 * - GIDMappings: 用户组 ID 映射列表
 */
struct Mount {
    std::string Destination;
    std::string Type;
    std::string Source;
    std::string Options;
	vector<LinuxIDMapping> UIDMappings;
	vector<LinuxIDMapping> GIDMappings;
};

#endif // DEFINE_SPECS_H
