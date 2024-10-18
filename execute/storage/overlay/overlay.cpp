#include "storage/overlay/overlay.h"

// parseOptions函数定义
std::shared_ptr<OverlayOptions> parseOptions(const std::vector<std::string>& options) {
    auto o = std::make_shared<OverlayOptions>();

    // 如果没有选项，直接返回
    if (options.empty()) {
        return o;
    }

    // 后续添加选项对应的处理逻辑
    for (const auto& option : options) {
        // 示例处理：根据选项更新 OverlayOptions
        // 这里的逻辑需要根据您的需求来实现
        // 例如：解析不同的选项并设置 OverlayOptions 的属性
    }

    // 返回构建好的 OverlayOptions
    return o;
}
string Join(const vector<string>& elem);
// 定义 mountProgramFlagFile 常量
const std::string mountProgramFlagFile = ".has-mount-program";

// getMountProgramFlagFile 函数的实现，使用 Join 函数
std::string getMountProgramFlagFile(const std::string& path) {
    // 将路径和文件名作为元素放入 vector 中
    std::vector<std::string> paths = {path, mountProgramFlagFile};
    // 使用 Join 函数拼接路径
    return Join(paths);
}
std::string trim_copy(const std::string& input) {
    std::string result = input;      // 创建一个副本
    boost::trim(result);             // 使用 boost::trim 去除首尾空白
    return result;                   // 返回处理后的字符串
}
// SupportsNativeOverlay函数实现
bool SupportsNativeOverlay(const std::string& home, const std::string& runhome) {
    try {
        // 1. 检查用户ID是否为0以及路径是否为空
        // if (getuid() != 0 || home.empty() || runhome.empty()) {
        //     return false; // 返回false，表示不支持
        // }

        // 2. 读取挂载程序标志文件内容
        std::string contents;
        std::string flagFilePath = getMountProgramFlagFile(home);
        if (boost::filesystem::exists(flagFilePath)) {
            std::ifstream flagFile(flagFilePath);
            if (flagFile.is_open()) {
                std::string line;
                std::getline(flagFile, line);
                contents = trim_copy(line);
            }
        }

        // 3. 根据标志文件的内容判断
        if (contents == "true") {
            // 如果已经配置了挂载程序
            // std::cout << "overlay: storage already configured with a mount-program" << std::endl;
            return false; // 返回false，表示不支持
        }

        // 4. 扫描是否需要挂载程序
        // bool needsMountProgram = scanForMountProgramIndicators(home);
        // if (boost::filesystem::exists(flagFilePath)) {
        //     std::ofstream outFile(flagFilePath);
        //     if (outFile.is_open()) {
        //         outFile << (needsMountProgram ? "true" : "false");
        //         outFile.close();
        //     }
        // }

        // if (needsMountProgram) {
        //     return false; // 返回false，表示不支持
        // }

        // 5. 检查路径是否存在，不存在则创建目录
        // for (const auto& dir : {home, runhome}) {
        //     if (!boost::filesystem::exists(dir)) {
        //         MkdirAllAs(dir, 0700, 0, 0); // 创建目录
        //     }
        // }

        // 6. 获取文件系统的类型并检查是否支持
        // auto fsMagic = GetFSMagic(home);
        // bool supportsDType = checkAndRecordOverlaySupport(fsMagic, home, runhome);
        // return supportsDType; // 返回文件系统支持的结果

    } catch (const myerror& e) {
        std::cerr << e.what() << std::endl;
        return false; // 捕获异常，返回false
    }

    // 默认返回false
    return false;
}




bool isExecutable(const boost::filesystem::path& file) {
    return boost::filesystem::exists(file) &&
           boost::filesystem::is_regular_file(file) &&
           (boost::filesystem::status(file).permissions() & boost::filesystem::perms::owner_exe);
}
std::string LookPath(const std::string& file) {
    try {
        // 如果 file 包含路径分隔符，则直接检查这个路径
        if (file.find("/") != std::string::npos) {
            boost::filesystem::path filePath(file);
            if (isExecutable(filePath)) {
                return filePath.string();
            }
            throw myerror("文件不可执行或未找到: " + file);
        }

        // 查找 PATH 环境变量
        const char* pathEnv = std::getenv("PATH");
        if (pathEnv == nullptr) {
            throw myerror("未设置 PATH 环境变量");
        }

        std::string path(pathEnv);
        std::vector<std::string> directories;
        boost::algorithm::split(directories, path, boost::is_any_of(":"));

        // 遍历 PATH 中的目录，查找可执行文件
        for (const auto& dir : directories) {
            boost::filesystem::path filePath = boost::filesystem::path(dir) / file;
            if (isExecutable(filePath)) {
                return boost::filesystem::absolute(filePath).string();
            }
        }

        return "fuse-overlayfs";
    } catch (const myerror& e) {
        throw;  // 只处理 myerror 类型的异常
    } catch (const std::exception& e) {
        throw myerror("未知错误: " + std::string(e.what()));
    }
}
std::shared_ptr<LayerIDMapUpdater_interface> NewNaiveLayerIDMapUpdater(std::shared_ptr<ProtoDriver_interface> protoDriver) {
    // 创建一个 naiveLayerIDMapUpdater 实例并返回它的 shared_ptr
    return std::make_shared<naiveLayerIDMapUpdater>(protoDriver); // 确保这里返回的是 std::shared_ptr
}

// 文件系统魔术值到名称的映射
std::map<FsMagic, std::string> FsNames = {
    {0x61756673, "aufs"},
    {0x9123683E, "btrfs"},
    {0x0000EF53, "extfs"},
    // 可以继续补充其他文件系统
};
Driver Init(const std::string& home, const driver_Options& options) {
    try {
        // 解析选项
        auto opts = parseOptions(options.driverOptions);
        
        // 获取文件系统魔术值
        auto fsMagic = GetFSMagic(home);
        std::string fsName = FsNames.count(fsMagic) ? FsNames[fsMagic] : "<unknown>";
        
        boost::filesystem::path runhomePath = boost::filesystem::path(options.runRoot) / boost::filesystem::path(home).filename();
        std::string runhome = runhomePath.make_preferred().string(); // 使用make_preferred()确保路径分隔符一致

        int rootUID=0, rootGID=0;
        const string linkDir = "l";
        // 创建驱动程序主目录
        if (!MkdirAllAs(Join({home, linkDir}), 0755, 0, 0)) {
            throw myerror("Failed to create directory for home");
        }

        // // if (!options.ImageStore.empty() && !idtools::MkdirAllAs(options.ImageStore + "/" + linkDir, 0755, 0, 0)) {
        //     throw myerror("Failed to create directory for image store");
        // }

        if (!MkdirAllAs(runhome, 0700, rootUID, rootGID)) {
            throw myerror("Failed to create directory for runhome");
        }

        // 检查 mountProgram
        if (opts->mountProgram.empty()) {
            if (!SupportsNativeOverlay(home, runhome)) {
                auto path = LookPath("fuse-overlayfs");
                if (!path.empty()) {
                    opts->mountProgram = path;
                }
            }
        }

        // 处理 mountProgram 相关逻辑
        if (!opts->mountProgram.empty()) {
            // if (unshare::IsRootless() && isNetworkFileSystem(fsMagic) && opts.forceMask == nullptr) {
            //     opts->forceMask = std::make_unique<int>(0700);
            //     logrus::Warn("Network file system detected as backing store. Enforcing overlay option `force_mask=\"%o\"`.", *opts.forceMask);
            // }
            // std::ofstream(getMountProgramFlagFile(home));
        // } else {
        //     if (opts->forceMask != nullptr) {
        //         throw myerror("'force_mask' is supported only with 'mount_program'");
        //     }
        //     // 检查不兼容文件系统
        //     switch (fsMagic) {
        //         case graphdriver::FsMagicAufs:
        //         case graphdriver::FsMagicOverlay:
        //         case graphdriver::FsMagicEcryptfs:
        //             throw myerror(fmt::format("'overlay' is not supported over {}, a mount_program is required", fsName));
        //     }
        //     if (unshare::IsRootless() && isNetworkFileSystem(fsMagic)) {
        //         throw myerror("A network file system with user namespaces is not supported. Please use a mount_program");
        //     }
        }

        // 检查 composefs 支持
        if (opts->useComposefs) {
            // if (unshare::IsRootless()) {
            //     throw myerror("composefs is not supported in user namespaces");
            // }
            // if (!supportsDataOnlyLayersCached(home, runhome)) {
            //     throw myerror("composefs is not supported on this kernel");
            // }
            // if (!getComposeFsHelper()) {
            //     throw myerror("composefs helper program not found");
            // }
        }

        bool usingMetacopy = false;
        bool supportsDType = false;
        std::shared_ptr<bool> supportsVolatile;

        if (!opts->mountProgram.empty()) {
            supportsDType = true;
            supportsVolatile = std::make_shared<bool>(true);
        } else {
            // supportsDType = checkAndRecordOverlaySupport(fsMagic, home, runhome);
            // std::string feature = fmt::format("metacopy({})", opts.mountOptions);
            // auto metacopyCacheResult = cachedFeatureCheck(runhome, feature);
            // usingMetacopy = metacopyCacheResult;
            // cachedFeatureRecord(runhome, feature, usingMetacopy, "");
        }

        // 挂载逻辑,不影响参数设置，如果需要回头再来考虑，并且最大的问题，unix和windows差距太大
        // if (!opts->skipMountHome) {
        //     mount::MakePrivate(home);
        // }
    FsMagic fileSystemType = FsMagicOverlay; 

    if (!opts->mountProgram.empty()) {
        fileSystemType = FsMagicFUSE;
    }

        // 在函数中进行 Driver 的初始化
        Driver d;
        d.name = "overlay";
        d.home = home;
        d.imageStore = options.imageStore;
        d.runhome = runhome;

        // 如果你需要 UID 和 GID 映射，你可以根据需求初始化它们
        // 假设 options.UIDMaps 和 options.GIDMaps 是一些整数值的向量
        // d.uidMaps = options.UIDMaps;
        // d.gidMaps = options.GIDMaps;

        // 初始化其他字段
        d.ctr = std::make_shared<RefCounter>(NewFsChecker(fileSystemType));
        d.supportsDType = supportsDType;
        d.usingMetacopy = usingMetacopy;
        d.supportsVolatile = supportsVolatile;
        d.usingComposefs = opts->useComposefs;
        d.options = *opts;

        // 初始化 naiveDiffDriver
        std::shared_ptr<ProtoDriver_interface> protoDriverPtr = std::shared_ptr<ProtoDriver_interface>(&d); // 使用 shared_ptr 包装
        std::shared_ptr<LayerIDMapUpdater_interface> updaterPtr = NewNaiveLayerIDMapUpdater(protoDriverPtr); // 确保返回 std::shared_ptr

        d.naiveDiff = std::make_shared<NaiveDiffDriver>(protoDriverPtr, updaterPtr);

        return d;
    } catch (const myerror& e) {
        throw;  // 重新抛出myerror
    }
}