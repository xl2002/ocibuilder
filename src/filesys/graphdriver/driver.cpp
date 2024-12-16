#include "filesys/graphdriver/driver.h"

using namespace std;
namespace fs = boost::filesystem; // 用于路径操作
const char Separator = '/';
// 清理路径，确保路径规范化
const std::string lowerFile = "lowers";
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


std::string Driver::String() {
    return this->name;
}
void Driver::Method2() {

}
void Driver::UpdateLayerIDMap(string& id) {

}
/**
 * @brief 在Driver类的home目录下创建子目录id，仅仅得到子目录的路径
 * 
 * @param id 
 * @param useImageStore 
 * @return std::tuple<std::string,std::string,bool> 
 */
// dir2 函数实现
std::tuple<std::string, std::string, bool> Driver::dir2(std::string& id, bool useImageStore) {
    std::string homedir;

    try {
        // 选择主目录路径
        if (useImageStore && !imageStore.empty()) {
            homedir = (fs::path(imageStore) / name).string();  // 使用 imageStore + name 作为主目录
        } else {
            homedir = home;  // 使用默认的 home 目录
        }

        // 构建目标路径
        std::string newpath = (fs::path(homedir) / id).string();  // 目标路径：home/id

        // 检查目标路径是否存在
        if (!fs::exists(newpath)) {
            // 如果目标路径不存在，遍历备用的镜像存储路径
            for (const auto& p : AdditionalImageStores()) {
                std::string backupPath = (fs::path(p) / name / id).string();
                if (fs::exists(backupPath)) {
                    return std::make_tuple(backupPath, homedir, true);  // 找到路径并返回
                }
            }
        }

        // 如果目标路径存在或没有找到备用路径，返回原始路径
        return std::make_tuple(newpath, homedir, false);

    } catch (const myerror& e) {
        throw;  // 抛出 myerror 异常
    } catch (const std::exception& e) {
        // 处理其他异常
        throw myerror("Unexpected error: " + std::string(e.what()));
    }
}
// getLower函数实现
std::string getLower(const std::string& parent) {
    try {
        // 获取父目录路径
        std::string parent_s = parent;
        Driver driver;  // 假设已经有一个 Driver 类的实例
        auto [parentDir, homedir, useImageStore] = driver.dir2(parent_s, false);  // 通过实例调用 dir2


        // 确保父目录存在
        if (!boost::filesystem::exists(parentDir)) {
            throw myerror("Parent directory does not exist: " + parentDir);
        }

        // 读取父目录中的 link 文件
        boost::filesystem::path parentLinkFile = parentDir + "/link";
        std::string parentLink;
        try {
            if (boost::filesystem::exists(parentLinkFile)) {
                std::ifstream linkStream(parentLinkFile.string());
                std::getline(linkStream, parentLink);
                linkStream.close();
            } else {
                throw myerror("Parent link file does not exist: " + parentLinkFile.string());
            }
        } catch (const std::exception& e) {
            throw myerror("Error reading parent link file: " + std::string(e.what()));
        }

        // 将父 link 添加到 lowers 列表
        std::string linkDir = parentDir + "/link";
        std::vector<std::string> lowers;
        lowers.push_back(linkDir + "/" + parentLink);

        // 读取 parentDir 中的 lowerFile 文件
        boost::filesystem::path parentLowerFile = parentDir + "/" + lowerFile;
        try {
            if (boost::filesystem::exists(parentLowerFile)) {
                std::ifstream lowersStream(parentLowerFile.string());
                std::string parentLower;
                std::getline(lowersStream, parentLower);
                lowersStream.close();

                // 拆分并将每个项添加到 lowers 列表
                std::istringstream ss(parentLower);
                std::string lower;
                while (std::getline(ss, lower, ':')) {
                    lowers.push_back(lower);
                }
            }
        } catch (const std::exception& e) {
            throw myerror("Error reading lowers file: " + std::string(e.what()));
        }

        // 将 lowers 列表合并为字符串并返回
        std::ostringstream result;
        for (size_t i = 0; i < lowers.size(); ++i) {
            result << lowers[i];
            if (i != lowers.size() - 1) {
                result << ":";
            }
        }
        return result.str();
    } catch (const myerror& e) {
        // 捕获并重新抛出自定义错误
        throw;
    } catch (const std::exception& e) {
        // 捕获所有标准异常并包装为 myerror
        throw myerror("An unexpected error occurred in getLower: " + std::string(e.what()));
    }
}
/**
 * @brief 创建oDriver.home下的目录结构
 *  
 * @param id 文件夹名
 * @param parent id对应层的父层
 * @param readOnly 
 */
void Driver::create(const std::string& id, const std::string& parent, std::shared_ptr<CreateOpts> opts, bool readOnly) {
    try {
        // 1. 通过 dir2 得到子目录的路径
        std::string dir;
        std::string homedir;
        std::tie(dir, homedir, std::ignore) = dir2(const_cast<std::string&>(id), readOnly); 

        // 2. 获取 UID 和 GID 映射（使用 opts 中的映射）
        int rootUID = 0;
        int rootGID = 0;

        if (opts && opts->IdMappings) {
            const auto& uidMappings = opts->IdMappings->UIDs();
            const auto& gidMappings = opts->IdMappings->GIDs();

            if (!uidMappings.empty()) {
                rootUID = uidMappings.front().ContainerID;  // 获取第一个 UID 映射
            }

            if (!gidMappings.empty()) {
                rootGID = gidMappings.front().ContainerID;  // 获取第一个 GID 映射
            }
        }

        // 3. 创建 dir 目录并设置权限和所有者
        MkdirAllAndChownNew(dir, 0700, rootUID, rootGID);

        // 4. 使用 MkdirAs 创建 dir/diff 子目录，并设置权限
        std::string diffDir = dir + "/diff";
        if (!mkdirAs(diffDir, 0700, rootUID, rootGID, true, false)) {
            throw myerror("Failed to create 'diff' directory.");
        }

        // 5. 使用 MkdirAs 创建 dir/merged 子目录，并设置权限
        std::string mergedDir = dir + "/merged";
        if (!mkdirAs(mergedDir, 0700, rootUID, rootGID, true, false)) {
            throw myerror("Failed to create 'merged' directory.");
        }

        // // 6. 创建符号链接
        // std::string lid = generateID();
        // std::string linkBase = "../" + id + "/diff";
        // boost::filesystem::path linkPath = homedir + "/link/" + lid;
        // boost::filesystem::path targetPath = linkBase;

        // try {
        //     if (boost::filesystem::exists(linkPath)) {
        //         throw myerror("Symlink already exists.");
        //     }
        //     boost::filesystem::create_symlink(targetPath, linkPath); // 使用Boost创建符号链接
        // }
        // catch (const boost::filesystem::filesystem_error& e) {
        //     throw myerror("Failed to create symlink: " + std::string(e.what()));
        // }

        // // 7. 写入链接 ID 到 link 文件
        // std::ofstream linkFile(dir + "/link");
        // if (!linkFile) {
        //     throw myerror("Failed to open link file for writing.");
        // }
        // linkFile << lid; // 写入lid内容
        // linkFile.close();
        // 8. 如果有 parent，处理 getLower
        // if (!parent.empty()) {
        //     std::string lower;
        //     try {
        //         lower = getLower(parent);
        //         std::ofstream lowerFile(dir + "/lowers");
        //         if (!lowerFile) {
        //             throw myerror("Failed to open lowers file for writing.");
        //         }
        //         lowerFile << lower; // 写入parent的lower信息
        //         lowerFile.close();
        //     }
        //     catch (const myerror& e) {
        //         throw myerror("Error in getLower: " + std::string(e.what()));
        //     }
        // }
    }
    catch (const std::exception& e) {
        throw myerror("Failed to create required directories.");
    }
}
/**
 * @brief 根据id创建layer层
 * 
 * @param id 
 * @param parent 
 * @param opts 
 */
void Driver::CreateReadWrite(const std::string& id, const std::string& parent, std::shared_ptr<CreateOpts> opts) {
    try {
        // 1. 检查存储选项是否符合条件
        if (opts != nullptr && !opts->StorageOpt.empty() ) {
            throw myerror("--storage-opt is supported only for overlay over xfs with 'pquota' mount option");
        }

        // 2. 如果 opts 为 nullptr，初始化为默认值
        if (opts == nullptr) {
            opts = std::make_shared<CreateOpts>();
            opts->StorageOpt = std::map<std::string, std::string>();
        }

        // 3. 设置默认存储空间大小 (size)
        if (opts->StorageOpt.find("size") == opts->StorageOpt.end()) {
            opts->StorageOpt["size"] = std::to_string(options.quota.Size);
        }

        // 4. 设置默认 inode 数量 (inodes)
        if (opts->StorageOpt.find("inodes") == opts->StorageOpt.end()) {
            opts->StorageOpt["inodes"] = std::to_string(options.quota.Inodes);
        }

        // 5. 调用底层的 create 方法来实际创建层
        create(id, parent, opts, false); // false 表示创建可读写层

    } catch (const myerror& e) {
        // 捕获 myerror 异常并输出错误信息
        throw;  // 重新抛出 myerror 异常
    } catch (const std::exception& e) {
        // 捕获其他标准异常
        throw myerror("Failed to create read-write layer."); // 将其他异常转换为 myerror 类型异常
    }
}




