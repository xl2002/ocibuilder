#include "platforms/database.h"
// 判断当前架构是否为ARM
bool isArmArch(const std::string& arch) {
    return arch == "arm" || arch == "arm64";
}
std::string GetWindowsOsVersion() {
    // 返回空字符串，实际应用中可以调用Windows API来获取OS版本
    return "";
}
bool isKnownOS(const std::string& os) {
    // 使用简单的字符串比较来检查已知的操作系统
    if (os == "aix" || os == "android" || os == "darwin" || os == "dragonfly" ||
        os == "freebsd" || os == "hurd" || os == "illumos" || os == "ios" ||
        os == "js" || os == "linux" || os == "nacl" || os == "netbsd" ||
        os == "openbsd" || os == "plan9" || os == "solaris" || os == "windows" ||
        os == "zos") {
        return true;
    }
    return false;
}
bool isKnownArch(const std::string& arch) {
    if (arch == "386" || arch == "amd64" || arch == "amd64p32" || arch == "arm" ||
        arch == "armbe" || arch == "arm64" || arch == "arm64be" || arch == "ppc64" ||
        arch == "ppc64le" || arch == "loong64" || arch == "mips" || arch == "mipsle" ||
        arch == "mips64" || arch == "mips64le" || arch == "mips64p32" || arch == "mips64p32le" ||
        arch == "ppc" || arch == "riscv" || arch == "riscv64" || arch == "s390" ||
        arch == "s390x" || arch == "sparc" || arch == "sparc64" || arch == "wasm") {
        return true;
    }
    return false;
}
// normalizeOS 函数
std::string normalizeOS(const std::string& os) {
    std::string normalizedOS = os.empty() ? "runtime_OS_placeholder" : os;  // 假设"runtime_OS_placeholder"为运行时OS

    boost::algorithm::to_lower(normalizedOS);

    if (normalizedOS == "macos") {
        normalizedOS = "darwin";
    }

    return normalizedOS;
}

// normalizeArch 函数
std::pair<std::string, std::string> normalizeArch(const std::string& arch, const std::string& variant) {
    std::string normalizedArch = arch;
    std::string normalizedVariant = variant;

    boost::algorithm::to_lower(normalizedArch);
    boost::algorithm::to_lower(normalizedVariant);

    if (normalizedArch == "i386") {
        normalizedArch = "386";
        normalizedVariant.clear();
    } else if (normalizedArch == "x86_64" || normalizedArch == "x86-64" || normalizedArch == "amd64") {
        normalizedArch = "amd64";
        if (normalizedVariant == "v1") {
            normalizedVariant.clear();
        }
    } else if (normalizedArch == "aarch64" || normalizedArch == "arm64") {
        normalizedArch = "arm64";
        if (normalizedVariant == "8" || normalizedVariant == "v8") {
            normalizedVariant.clear();
        }
    } else if (normalizedArch == "armhf") {
        normalizedArch = "arm";
        normalizedVariant = "v7";
    } else if (normalizedArch == "armel") {
        normalizedArch = "arm";
        normalizedVariant = "v6";
    } else if (normalizedArch == "arm") {
        if (normalizedVariant.empty() || normalizedVariant == "7") {
            normalizedVariant = "v7";
        } else if (normalizedVariant == "5" || normalizedVariant == "6" || normalizedVariant == "8") {
            normalizedVariant = "v" + normalizedVariant;
        }
    }

    return {normalizedArch, normalizedVariant};
}