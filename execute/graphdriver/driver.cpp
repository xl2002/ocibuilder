#include "graphdriver/driver.h"

using namespace std;
const char Separator = '/';
// 清理路径，确保路径规范化
std::unordered_map<std::string, std::function<std::shared_ptr<Driver>(const std::string&, const driver_Options&)>> drivers;
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

    for (const auto& [driver, _] : drivers) {
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
}
std::vector<std::string> Driver::AdditionalImageStores(){
    return options.imageStores;
}