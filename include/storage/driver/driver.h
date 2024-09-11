#if !defined(SORAGE_DRIVER_H)
#define SORAGE_DRIVER_H
#include <string>
#include <functional>
#include "cobra/error.h"
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


// 定义错误信息
const string ErrNotSupported = "driver not supported";
const string ErrPrerequisites = "prerequisites for driver not satisfied (wrong filesystem?)";
const string ErrIncompatibleFS = "backing file system is unsupported for this graph driver";
const string ErrLayerUnknown = "unknown layer";
// const myerror ErrNotSupported = myerror("driver not supported");
// const myerror ErrPrerequisites = myerror("prerequisites for driver not satisfied (wrong filesystem?)");
// const myerror ErrIncompatibleFS = myerror("backing file system is unsupported for this graph driver");
// const myerror ErrLayerUnknown = myerror("unknown layer");

class ProtoDriver {
public:
    virtual ~ProtoDriver() {}
    virtual string String() = 0;
};

// 定义DiffDriver抽象基类
class DiffDriver {
public:
    virtual ~DiffDriver() {}
    virtual void Diff(string& id, string& parent) = 0;
};

// 定义LayerIDMapUpdater抽象基类
class LayerIDMapUpdater {
public:
    virtual ~LayerIDMapUpdater() {}
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
    bool withReference;        // 是否包含图像引用信息
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
    bool skipMountHome;                               // 是否跳过挂载主目录
    std::string mountOptions;                         // 挂载选项
    bool ignoreChownErrors;                           // 是否忽略 chown 错误
    std::shared_ptr<FileMode> forceMask;              // 强制掩码（文件权限模式）
    bool useComposefs;                                // 是否使用 composefs
};
// Driver 类定义
class Driver : public ProtoDriver, public DiffDriver, public LayerIDMapUpdater {
public:
    virtual ~Driver() {}

    // ProtoDriver 的实现
    std::string String() override {
        return "MyDriver"; // 示例实现
    }
    std::vector<std::string> AdditionalImageStores();
public:
    std::string name;  // Driver 名称
    std::string home;  // home 目录
    std::string runhome; // runhome 目录
    std::string imageStore; // imageStore 目录
    // std::vector<std::string> uidMaps; // UID 映射
    // std::vector<std::string> gidMaps; // GID 映射
    // std::shared_ptr<RefCounter> ctr; // RefCounter
    // std::shared_ptr<Control> quotaCtl; // Control
    OverlayOptions options; // overlayOptions
    std::shared_ptr<DiffDriver> naiveDiff; // DiffDriver
    bool supportsDType; // 是否支持 DType
    std::shared_ptr<bool> supportsVolatile; // 是否支持 Volatile
    bool usingMetacopy; // 是否使用 Metacopy
    bool usingComposefs; // 是否使用 Composefs
    std::shared_ptr<bool> supportsIDMappedMounts; // 是否支持 ID 映射挂载
};
typedef struct Options {
    string root;                  // 根目录
    string runRoot;               // 运行目录
    string imageStore;            // 镜像存储目录
    vector<string> driverPriority; // 驱动优先级
    vector<string> driverOptions;  // 驱动选项
    // vector<IDMap> uidMaps;         // UID 映射
    // vector<IDMap> gidMaps;         // GID 映射
    bool experimentalEnabled;     // 是否启用实验特性
}Options;
// 使用unordered_map存储所有注册的驱动程序
shared_ptr<Driver> GetDriver(const string& name, const Options& config);
unordered_map<string, bool> ScanPriorDrivers(const string& root);
shared_ptr<Driver> getBuiltinDriver(const std::string& name, const std::string& home, const Options& options);
#endif // SORAGE_DRIVER_H
