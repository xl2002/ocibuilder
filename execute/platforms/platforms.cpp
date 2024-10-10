#include "platforms/platforms.h"

Platform Normalize(const Platform& platform) {
    Platform normalizedPlatform = platform;

    // 规范化操作系统
    normalizedPlatform.OS = normalizeOS(platform.OS);

    // 规范化架构和变体
    std::tie(normalizedPlatform.Architecture, normalizedPlatform.Variant) = 
        normalizeArch(platform.Architecture, platform.Variant);

    return normalizedPlatform;
}

Platform Parse(const std::string& specifier) {
    try {
        if (specifier.find('*') != std::string::npos) {
            throw myerror("Wildcards not yet supported: " + specifier);
        }

        std::vector<std::string> parts;
        boost::split(parts, specifier, boost::is_any_of("/"));

        for (const auto& part : parts) {
            // if (!std::regex_match(part, specifierRe)) {
            //     throw myerror(part + " is an invalid component of " + specifier + 
            //                    ": platform specifier component must match: " + specifierRe.str() +
            //                    " : " + std::to_string(errdefs::ErrInvalidArgument));
            // }
        }

        Platform p;
        switch (parts.size()) {
            case 1: {
                p.OS = normalizeOS(parts[0]);
                if (isKnownOS(p.OS)) {
                    p.Architecture = "arm"; // 这里假设使用默认值，您可以根据需要更改
                    if (p.OS == "windows") {
                        p.OSVersion = GetWindowsOsVersion();
                    }
                    return p;
                }

                auto archVariant = normalizeArch(parts[0], "");
                p.Architecture = archVariant.first;
                p.Variant = archVariant.second;
                if (isKnownArch(p.Architecture)) {
                    p.OS = "linux"; // 假设的默认值
                    return p;
                }
                throw myerror(specifier + ": unknown operating system or architecture: " );
            }
            case 2: {
                p.OS = normalizeOS(parts[0]);
                auto archVariant = normalizeArch(parts[1], "");
                p.Architecture = archVariant.first;
                p.Variant = archVariant.second;
                if (p.OS == "windows") {
                    p.OSVersion = GetWindowsOsVersion();
                }
                return p;
            }
            case 3: {
                p.OS = normalizeOS(parts[0]);
                auto archVariant = normalizeArch(parts[1], parts[2]);
                p.Architecture = archVariant.first;
                p.Variant = archVariant.second;
                if (p.Architecture == "arm64" && p.Variant.empty()) {
                    p.Variant = "v8";
                }
                if (p.OS == "windows") {
                    p.OSVersion = GetWindowsOsVersion();
                }
                return p;
            }
            default:
                throw myerror(specifier + ": cannot parse platform specifier: " );
        }
    } catch (const myerror& e) {
        // 处理 myerror 类型的异常
        throw; // 重新抛出异常
    }
}