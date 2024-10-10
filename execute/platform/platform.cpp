#include "platform/platform.h"

Platform NormalizePlatform(const std::string& rawOS, const std::string& rawArch, const std::string& rawVariant) {
    try {
        Platform platformSpec(rawArch, rawOS, rawVariant);

        // 假设有一个外部的 Normalize 函数来规范化平台信息
        Platform normalizedSpec = Normalize(platformSpec); // 你可能需要根据你实际的规范化逻辑进行替换

        // 如果 Variant 为空且原始 Variant 不为空，则保留原始 Variant
        if (normalizedSpec.Variant.empty() && !rawVariant.empty()) {
            normalizedSpec.Variant = rawVariant;
        }

        // 将原始平台转换为字符串并解析回规范化后的平台
        std::string rawPlatform = ToString(normalizedSpec.OS, normalizedSpec.Architecture, normalizedSpec.Variant);
        Platform normalizedPlatform = Parse(rawPlatform); // 同样需要用你实际的 Parse 逻辑

        return Platform{
            rawOS.empty() ? normalizedPlatform.OS : rawOS,
            rawArch.empty() ? normalizedPlatform.Architecture : rawArch,
            rawVariant.empty() && !normalizedPlatform.Variant.empty() ? normalizedPlatform.Variant : rawVariant
        };
    } catch (const myerror& e) {
        // 处理 myerror 类型的异常
        throw; // 重新抛出异常
    }
}

// ToString函数，生成格式化的字符串
std::string ToString(const std::string& os, const std::string& arch, const std::string& variant) {
    std::string osString = os.empty() ? "unknownOS" : os;
    std::string archString = arch.empty() ? "unknownArch" : arch;

    if (variant.empty()) {
        return osString + "/" + archString;
    }
    return osString + "/" + archString + "/" + variant;
}