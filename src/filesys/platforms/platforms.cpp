#include "filesys/platforms/platforms.h"
#include "filesys/platforms/database.h"
#include "utils/common/error.h"
#include "filesys/platforms/default_unix.h"
// static std::regex specifierRe("[a-zA-Z0-9]+");
/**
 * @brief 标准化平台规范对象
 * @param platform 要标准化的平台对象指针
 * @return 标准化后的平台对象指针
 * @note 会调用normalizeOS和normalizeArch对OS和Architecture进行标准化
 */
std::shared_ptr<Platform> Normalize(std::shared_ptr<Platform> platform) {
    platform->OS=normalizeOS(platform->OS);
    std::tie(platform->Architecture,platform->Variant)=normalizeArch(platform->Architecture,platform->Variant);
    return platform;
}
/**
 * @brief 解析平台规范字符串
 * @param specifier 平台规范字符串，格式为"os/arch/variant"或"os/arch"或"os"
 * @return 解析后的平台对象指针
 * @throw myerror 如果规范无效或包含未知操作系统/架构
 * @note 支持1-3部分格式：
 *       1部分: "os"或"arch"
 *       2部分: "os/arch"
 *       3部分: "os/arch/variant"
 */
std::shared_ptr<Platform> PlatForms::Parse(const std::string& specifier) {
    
    Platform p;

    if (specifier.find("*") != std::string::npos) {
        throw myerror("wildcards not yet supported");
    }

    std::vector<std::string> parts;
    std::istringstream iss(specifier);
    std::string part;
    while (std::getline(iss, part, '/')) {
        parts.push_back(part);
    }

    for (const auto& part : parts) {
        if (!std::regex_match(part, specifierRe)) {
            throw myerror(part + " is an invalid component of " + specifier);
        }
    }

    switch (parts.size()) {
        case 1:
            p.OS = normalizeOS(parts[0]);
            if (isKnownOS(p.OS)) {
                p.Architecture = "runtime_arch"; // 模拟当前架构
                if (p.Architecture == "arm" && cpuVariant() != "v7") {
                    p.Variant = cpuVariant();
                }
                if (p.OS == "windows") {
                    p.OSVersion = GetWindowsOsVersion();
                }
                return std::make_shared<Platform>(p);
            }
            std::tie(p.Architecture, p.Variant) = normalizeArch(parts[0], "");
            if (p.Architecture == "arm" && p.Variant == "v7") {
                p.Variant = "";
            }
            if (isKnownArch(p.Architecture)) {
                p.OS = "runtime_os"; // 模拟当前系统
                return std::make_shared<Platform>(p);
            }
            throw myerror("unknown operating system or architecture");
        case 2:
            p.OS = normalizeOS(parts[0]);
            std::tie(p.Architecture, p.Variant) = normalizeArch(parts[1], "");
            if (p.Architecture == "arm" && p.Variant == "v7") {
                p.Variant = "";
            }
            if (p.OS == "windows") {
                p.OSVersion = GetWindowsOsVersion();
            }
            return std::make_shared<Platform>(p);
        case 3:
            p.OS = normalizeOS(parts[0]);
            std::tie(p.Architecture, p.Variant) = normalizeArch(parts[1], parts[2]);
            if (p.Architecture == "arm64" && p.Variant == "") {
                p.Variant = "v8";
            }
            if (p.OS == "windows") {
                p.OSVersion = GetWindowsOsVersion();
            }
            return std::make_shared<Platform>(p);
        default:
            throw myerror("cannot parse platform specifier");
    }
}
