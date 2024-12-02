#if !defined(FILESYS_GRAPHDRIVER_DRIVER_H)
#define FILESYS_GRAPHDRIVER_DRIVER_H
#include <string>
#include <functional>
#include "utils/common/error.h"
#include "filesys/graphdriver/counter.h"
#include <memory>       // 用于 std::shared_ptr
#include <unordered_map> // 用于 std::unordered_map
#include <string>       // 用于 std::string
#include <stdexcept>    // 用于 std::runtime_error
#include <filesystem>   // 用于 std::filesystem::path, 以及路径拼接
#include <iostream>     // 用于输出错误日志 (可选)
#include <vector>
#include <sstream>
#include <algorithm>
#include <unistd.h> // For access function
using namespace std;
// 定义ProtoDriver抽象基类
// class Driver;

// 定义错误信息
extern string ErrNotSupported;
extern string ErrPrerequisites;
extern string ErrIncompatibleFS;
extern string ErrLayerUnknown;
// const myerror ErrNotSupported = myerror("driver not supported");
// const myerror ErrPrerequisites = myerror("prerequisites for driver not satisfied (wrong filesystem?)");
// const myerror ErrIncompatibleFS = myerror("backing file system is unsupported for this graph driver");
// const myerror ErrLayerUnknown = myerror("unknown layer");

class ProtoDriver_interface {
public:
    virtual ~ProtoDriver_interface()=default;
    virtual string String() = 0;
};

// 定义DiffDriver抽象基类
class DiffDriver_interface {
public:
    virtual ~DiffDriver_interface()=default;
    virtual void Method2() = 0;
};

// 定义LayerIDMapUpdater抽象基类
class LayerIDMapUpdater_interface {
public:
    virtual ~LayerIDMapUpdater_interface()=default;
    virtual void UpdateLayerIDMap(string& id) = 0;
};
// Quota 类
class Quota {
public:
    uint64_t Size;   // 阻止块硬限制
    uint64_t Inodes; // inode 限制
};

// additionalLayerStore 类
class AdditionalLayerStore {
public:
    std::string path;          // 存储目录的路径
    bool withReference=false;        // 是否包含图像引用信息
};

// FileMode 类型别名
typedef uint32_t FileMode;

// OverlayOptions 类
class OverlayOptions {
public:
    std::vector<std::string> imageStores;             // 镜像存储目录列表
    std::vector<AdditionalLayerStore> layerStores;    // 额外的层存储列表
    Quota quota;                                      // 配额
    std::string mountProgram;                         // 挂载程序
    bool skipMountHome=false;                               // 是否跳过挂载主目录
    std::string mountOptions;                         // 挂载选项
    bool ignoreChownErrors=false;                           // 是否忽略 chown 错误
    std::shared_ptr<FileMode> forceMask=std::make_shared<FileMode>();              // 强制掩码（文件权限模式）
    bool useComposefs=false;                                // 是否使用 composefs
};
// Driver 类定义
class Driver : public ProtoDriver_interface, public DiffDriver_interface, public LayerIDMapUpdater_interface {
public:
    ~Driver()=default;
    Driver()=default;
    Driver(const Driver& other) = default;
    // ProtoDriver_interface 的实现
    std::string String() override {
        return "MyDriver"; // 示例实现
    }
    void Method2()override {

    }
    void UpdateLayerIDMap(string& id)override {

    }
    std::vector<std::string> AdditionalImageStores();
public:
    std::string name;  // Driver 名称
    std::string home;  // home 目录
    std::string runhome; // runhome 目录
    std::string imageStore; // imageStore 目录
    // std::vector<std::string> uidMaps; // UID 映射
    // std::vector<std::string> gidMaps; // GID 映射
    std::shared_ptr<RefCounter> ctr; // RefCounter
    // std::shared_ptr<Control> quotaCtl; // Control
    OverlayOptions options; // overlayOptions
    std::shared_ptr<DiffDriver_interface> naiveDiff=nullptr; // DiffDriver_interface
    bool supportsDType=false; // 是否支持 DType
    std::shared_ptr<bool> supportsVolatile=std::make_shared<bool>(false); // 是否支持 Volatile
    bool usingMetacopy=false; // 是否使用 Metacopy
    bool usingComposefs=false; // 是否使用 Composefs
    std::shared_ptr<bool> supportsIDMappedMounts=std::make_shared<bool>(false); // 是否支持 ID 映射挂载
};
typedef struct driver_Options {
    string root;                  // 根目录
    string runRoot;               // 运行目录
    string imageStore;            // 镜像存储目录
    vector<string> driverPriority; // 驱动优先级
    vector<string> driverOptions;  // 驱动选项
    // vector<IDMap> uidMaps;         // UID 映射
    // vector<IDMap> gidMaps;         // GID 映射
    bool experimentalEnabled=false;     // 是否启用实验特性
    driver_Options() = default;
}driver_Options;
// 使用unordered_map存储所有注册的驱动程序
extern std::unordered_map<std::string, std::function<std::shared_ptr<Driver>(const std::string&, const driver_Options&)>> drivers;
shared_ptr<Driver> GetDriver(const string& name, const driver_Options& config);
unordered_map<string, bool> ScanPriorDrivers(const string& root);
shared_ptr<Driver> getBuiltinDriver(const std::string& name, const std::string& home, const driver_Options& options);
shared_ptr<Driver> New(const string& name, const driver_Options& config);
#endif // GRAPHDRIVER_DRIVER_H