#if !defined(FILESYS_UTILS_COPIER_H)
#define FILESYS_UTILS_COPIER_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sys/stat.h>
#include "filesys/utils/idtools.h"
// PutOptions 类用于控制 Put 行为的选项
class PutOptions {
public:
    std::vector<IDMap> UIDMap; // UID 映射
    std::vector<IDMap> GIDMap; // GID 映射

    std::shared_ptr<IDPair> DefaultDirOwner; // 隐式创建目录的默认所有者
    std::shared_ptr<mode_t> DefaultDirMode;  // 隐式创建目录的默认权限

    std::shared_ptr<IDPair> ChownDirs; // 新建目录的所有者
    std::shared_ptr<mode_t> ChmodDirs; // 新建目录的权限

    std::shared_ptr<IDPair> ChownFiles; // 新建文件的所有者
    std::shared_ptr<mode_t> ChmodFiles; // 新建文件的权限

    bool StripSetuidBit = false;       // 是否移除 setuid 位
    bool StripSetgidBit = false;       // 是否移除 setgid 位
    bool StripStickyBit = false;       // 是否移除粘滞位
    bool StripXattrs = false;          // 是否忽略扩展属性
    bool IgnoreXattrErrors = false;    // 是否忽略设置扩展属性时的错误
    bool IgnoreDevices = false;        // 是否忽略字符或块设备
    bool NoOverwriteDirNonDir = false; // 不允许目录与非目录互相覆盖
    bool NoOverwriteNonDirDir = false; // 不允许非目录与目录互相覆盖

    std::map<std::string, std::string> Rename; // 项目重命名规则

    // 默认构造函数
    PutOptions() = default;

    // 析构函数
    ~PutOptions() = default;
};

#endif // FILESYS_UTILS_COPIER_H)
