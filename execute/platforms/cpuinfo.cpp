#include "platforms/cpuinfo.h"
std::string cpuVariantValue;
std::once_flag cpuVariantFlag;
// 使用Boost::filesystem读取文件内容
std::string getCPUInfo(const std::string& pattern) {
    try {
        boost::filesystem::path cpuinfoPath("/proc/cpuinfo");
        if (!boost::filesystem::exists(cpuinfoPath)) {
            throw myerror("无法打开 /proc/cpuinfo");
        }

        std::ifstream cpuinfo(cpuinfoPath.string());
        std::string line;
        while (std::getline(cpuinfo, line)) {
            std::vector<std::string> parts;
            boost::split(parts, line, boost::is_any_of(":"));
            if (parts.size() > 1 && boost::trim_copy(parts[0]) == pattern) {
                return boost::trim_copy(parts[1]);
            }
        }
        throw myerror("无法找到CPU信息模式: " + pattern);
    } catch (...) {
        throw; // 重新抛出异常
    }
}

// 使用Boost::process获取机器架构
std::string getMachineArch() {
    try {
        boost::process::ipstream output; // 输出流
        boost::process::system("uname -m", boost::process::std_out > output);

        std::string arch;
        std::getline(output, arch); // 从输出流中读取
        boost::trim(arch); // 去除空白符

        if (arch.empty()) {
            throw myerror("无法获取机器架构");
        }
        return arch;
    } catch (...) {
        throw; // 重新抛出异常
    }
}

// 通过架构获取CPU Variant
std::string getCPUVariantFromArch(const std::string& arch) {
    try {
        std::string lowerArch = boost::to_lower_copy(arch);

        if (lowerArch == "aarch64") {
            return "8";
        } else if (lowerArch.substr(0, 4) == "armv" && lowerArch.size() >= 5) {
            std::string version = lowerArch.substr(4, 1);
            if (version >= "3" && version <= "8") {
                return version;
            } else {
                throw myerror("无效的ARM架构版本: " + arch);
            }
        } else {
            throw myerror("无效的架构: " + arch);
        }
    } catch (...) {
        throw; // 重新抛出异常
    }
}

// 获取CPU Variant
std::string getCPUVariant() {
    try {
        std::string variant = getCPUInfo("Cpu architecture");
        if (variant.empty()) {
            std::string arch = getMachineArch();
            variant = getCPUVariantFromArch(arch);
        }

        // 针对树莓派ARMv6的特殊处理
        if (variant == "7") {
            std::string model = getCPUInfo("model name");
            if (model.find("armv6") != std::string::npos) {
                variant = "6";
            }
        }

        return "v" + variant;
    } catch (...) {
        throw; // 重新抛出异常
    }
}


// cpuVariant函数的实现
std::string cpuVariant() {
    std::call_once(cpuVariantFlag, []() {
        try {
            std::string arch = getMachineArch(); // 获取机器架构
            if (isArmArch(arch)) {
                cpuVariantValue = getCPUVariant();
            }
        } catch (const myerror& e) {
            // 只需抛出异常，不输出错误信息
            throw; // 重新抛出异常
        }
    });

    return cpuVariantValue;
}