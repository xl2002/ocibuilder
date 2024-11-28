#if !defined(FILESYS_SYSTEM_DEVICES_H)
#define FILESYS_SYSTEM_DEVICES_H
#include <string>
#include <memory>
#include <vector>
#include <sys/stat.h>
#include <cstdint>
struct Permissions {
    std::string value;
};
class Type {
    public:
    char ch;
};
class Rule {
    public:
    // Type of device ('c' for char, 'b' for block). If set to 'a', this rule
    // acts as a wildcard and all fields other than Allow are ignored.
    std::shared_ptr<Type> type=std::make_shared<Type>();

    // Major is the device's major number.
    int64_t major=0;

    // Minor is the device's minor number.
    int64_t minor=0;

    // Permissions is the set of permissions that this rule applies to (in the
    // cgroupv1 format -- any combination of "rwm").
    std::shared_ptr<Permissions> permissions=std::make_shared<Permissions>();

    // Allow specifies whether this rule is allowed.
    bool allow=false;
};

class Device : public Rule {
    public:
    // Path to the device.
    std::string path;

    // FileMode permission bits for the device.
    mode_t fileMode;

    // Uid of the device.
    uint32_t uid=0;

    // Gid of the device.
    uint32_t gid=0;
};

#endif // DEVICES_DEVICES_H)
