#include "filesys/graphdriver/driver.h"

using namespace std;
const char Separator = '/';
// 清理路径，确保路径规范化

string Clean(const string& path);
string join(const vector<string>& elem);
string Join(const vector<string>& elem);
shared_ptr<Driver> GetDriver(const string& name, const driver_Options& config) {
    // GetDriver 初始化并返回已注册的驱动程序
    vector<string> pathElements = { config.root, name };

    // 使用 Join 函数连接路径
    string joinedPath = Join(pathElements);

    auto it = drivers.find(name);
    if (it != drivers.end()) {
        // 调用驱动程序初始化函数
        return it->second(joinedPath, config);
    }
    // 如果驱动程序未找到，记录错误日志
    // logError("Failed to GetDriver graph " + name + " " + config.Root);

    // 抛出myerror类型的错误，表示驱动程序未找到
    throw myerror("failed to GetDriver graph " + name + " " + config.root + ": " + "ErrNotSupported");
}
// 检查文件或目录是否存在
bool exists(const string& path) {
    return access(path.c_str(), F_OK) != -1; // F_OK 检查文件是否存在
}
// 扫描目录并返回已存在的驱动程序
unordered_map<string, bool> ScanPriorDrivers(const string& root) {
    unordered_map<string, bool> driversMap;

    for (const auto& ds : drivers) {
        auto driver = ds.first;
        vector<string> pathParts = {root, driver}; // 将 root 和 driver 分为路径部分
        string p = join(pathParts); // 使用 join 函数拼接路径
        if (exists(p)) {
            driversMap[driver] = true;
        }
    }

    return driversMap;
}
// getBuiltinDriver 函数实现
shared_ptr<Driver> getBuiltinDriver(const std::string& name, const std::string& home, const driver_Options& options) {
    auto it = drivers.find(name);
    if (it != drivers.end()) {
        // 使用 Join 函数连接路径，并调用驱动程序初始化函数
        return it->second(Join({home, name}), options);
    }

    // 如果驱动程序未找到，记录错误日志并抛出 myerror 类型的异常
    std::ostringstream errorMessage;
    errorMessage << "Failed to built-in GetDriver graph " << name << " " << home;
    // logError(errorMessage.str());
    
    std::ostringstream formattedMessage;
    formattedMessage << "failed to built-in GetDriver graph " << name << " " << home << ": " << ErrNotSupported;
    throw myerror(formattedMessage.str());

    // return nullptr;
}
std::vector<std::string> Driver::AdditionalImageStores(){
    return options.imageStores;
}
// 定义优先级驱动程序的向量
std::vector<std::string> Priority = {
    "overlay",
    // We don't support devicemapper without configuration
    // "devicemapper",
    "aufs",
    "btrfs",
    "zfs",
    "vfs"
};
shared_ptr<Driver> New(const string& name, const driver_Options& config) {
    if (!name.empty()) {
        // 如果指定了驱动名称，尝试加载指定的驱动，并记录到日志中,日志系统先不考虑
        // logDebug("[graphdriver] trying provided driver \"" + name + "\"");
        return GetDriver(name, config);
    }

    // 尝试加载之前使用的驱动
    auto driversMap = ScanPriorDrivers(config.root);

    // 使用提供的优先级列表，如果为空则使用默认的优先级列表
    vector<string> prioList = config.driverPriority;
    if (prioList.empty()) {
        prioList = Priority;
    }

    // 遍历优先级列表并尝试加载驱动
    for (const auto& name : prioList) {
        if (name == "vfs" && config.driverPriority.empty()) {
            // 如果优先级列表为空，跳过vfs驱动
            continue;
        }
        if (driversMap.find(name) != driversMap.end()) {
            // 如果在已加载的驱动中找到优先驱动，尝试加载并返回
            auto driver = getBuiltinDriver(name, config.root, config);
            if (!driver) {
                // 如果加载失败，记录错误日志并返回错误
                // logError("[graphdriver] prior storage driver " + name + " failed.");
                throw myerror("Failed to load driver: " + name);
            }

            // 如果有多个已加载的驱动，要求用户显式选择一个驱动
            if (driversMap.size() - 1 > 0) {
                vector<string> driversSlice;
                for (const auto& kv : driversMap) {
                    driversSlice.push_back(kv.first);
                }

                throw myerror(config.root + " contains several valid graphdrivers: " +
                                "; Please cleanup or explicitly choose storage driver (-s <DRIVER>)");
            }

            // logInfo("[graphdriver] using prior storage driver: " + name);
            return driver;
        }
    }

    // 按优先级列表顺序检查并加载驱动
    for (const auto& name : prioList) {
        auto driver = getBuiltinDriver(name, config.root, config);
        if (driver) {
            return driver;
        }
    }

    // 如果没有找到优先驱动，检查所有已注册的驱动
    for (auto& kv : drivers) {
        std::vector<std::string> pathComponents = {config.root, kv.first}; // 创建路径组件的向量
        std::string path = Join(pathComponents); // 连接路径
        driver_Options opts;
        opts.driverOptions = config.driverOptions; // 将现有的 vector<string> 分配给 Options 对象的 driverOptions
        auto driver = kv.second(path, opts); // 使用 Options 对象作为参数
        if (driver) {
            return driver;
        }
    }

    // 如果没有找到支持的存储驱动，抛出myerror类型的错误
    throw myerror("no supported storage backend found");
    // return nullptr;
}