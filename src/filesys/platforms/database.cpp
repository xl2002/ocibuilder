#include "filesys/platforms/database.h"
#include "filesys/platforms/default_unix.h"
/**
 * @brief 检查给定的操作系统名称是否为已知操作系统
 * @param os 要检查的操作系统名称
 * @return 如果操作系统已知返回true，否则返回false
 */
bool isKnownOS(const std::string& os) {
    static const std::vector<std::string> knownOS = {
        "aix", "android", "darwin", "dragonfly", "freebsd", "hurd", "illumos", 
        "ios", "js", "linux", "nacl", "netbsd", "openbsd", "plan9", 
        "solaris", "windows", "zos"
    };
    return std::find(knownOS.begin(), knownOS.end(), os) != knownOS.end();
}

/**
 * @brief 检查给定的架构是否为ARM架构
 * @param arch 要检查的架构名称
 * @return 如果是ARM架构返回true，否则返回false
 */
bool isArmArch(const std::string& arch) {
    return arch == "arm" || arch == "arm64";
}

/**
 * @brief 检查给定的架构名称是否为已知架构
 * @param arch 要检查的架构名称
 * @return 如果架构已知返回true，否则返回false
 */
bool isKnownArch(const std::string& arch) {
    static const std::vector<std::string> knownArch = {
        "386", "amd64", "amd64p32", "arm", "armbe", "arm64", "arm64be", "ppc64", 
        "ppc64le", "loong64", "mips", "mipsle", "mips64", "mips64le", 
        "mips64p32", "mips64p32le", "ppc", "riscv", "riscv64", "s390", 
        "s390x", "sparc", "sparc64", "wasm"
    };
    return std::find(knownArch.begin(), knownArch.end(), arch) != knownArch.end();
}

/**
 * @brief 将操作系统名称标准化为统一格式
 * @param os 要标准化的操作系统名称
 * @return 标准化后的操作系统名称
 * @note 会将"macos"转换为"darwin"，并将所有字符转为小写
 */
std::string normalizeOS(const std::string& os) {
    if (os.empty()) {
        return GOOS;  // 模拟运行时系统的操作系统
    }
    std::string lowerOS = os;
    std::transform(lowerOS.begin(), lowerOS.end(), lowerOS.begin(), ::tolower);

    if (lowerOS == "macos") {
        lowerOS = "darwin";
    }

    return lowerOS;
}

/**
 * @brief 将架构名称和变体标准化为统一格式
 * @param arch 要标准化的架构名称
 * @param variant 架构变体
 * @return 包含标准化后的架构和变体的元组
 * @note 处理多种架构别名，如i386->386, x86_64->amd64等
 */
std::tuple<std::string, std::string> normalizeArch(std::string arch, std::string variant) {
    std::transform(arch.begin(), arch.end(), arch.begin(), ::tolower);
    std::transform(variant.begin(), variant.end(), variant.begin(), ::tolower);

    if (arch == "i386") {
        arch = "386";
        variant = "";
    } else if (arch == "x86_64" || arch == "x86-64" || arch == "amd64") {
        arch = "amd64";
        if (variant == "v1") {
            variant = "";
        }
    } else if (arch == "aarch64" || arch == "arm64") {
        arch = "arm64";
        if (variant == "8" || variant == "v8") {
            variant = "";
        }
    } else if (arch == "armhf") {
        arch = "arm";
        variant = "v7";
    } else if (arch == "armel") {
        arch = "arm";
        variant = "v6";
    } else if (arch == "arm") {
        if (variant.empty() || variant == "7") {
            variant = "v7";
        } else if (variant == "5" || variant == "6" || variant == "8") {
            variant = "v" + variant;
        }
    }

    return std::make_tuple(arch, variant);
}
