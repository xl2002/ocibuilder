#include "cmd/build/imagebuilder/internals.h"
#include "utils/common/go/file.h"
#include "utils/logger/ProcessSafeLogger.h"
std::vector<std::string> mergeEnv(std::vector<std::string> defaults, std::vector<std::string> overrides){
    std::vector<std::string> result;
    result.reserve(defaults.size() + overrides.size());
    std::map<std::string, int> index;

    // 合并 defaults 和 overrides
    for (const auto& envSpec : defaults) {
        std::string key = envSpec.substr(0, envSpec.find('='));
        result.push_back(envSpec);
        index[key] = result.size() - 1;
    }

    for (const auto& envSpec : overrides) {
        std::string key = envSpec.substr(0, envSpec.find('='));
        auto it = index.find(key);
        if (it != index.end()) {
            result[it->second] = envSpec;  // 覆盖已有的环境变量
        } else {
            result.push_back(envSpec);  // 添加新的环境变量
            index[key] = result.size() - 1;
        }
    }

    return result;
}
// envMapAsSlice 返回一个包含 "key=value" 格式字符串的 vector
std::vector<std::string> envMapAsSlice(const std::map<std::string, std::string>& m) {
    std::vector<std::string> result;
    result.reserve(m.size());  // 提前分配空间以提高性能

    for (const auto& pair : m) {
        result.push_back(pair.first + "=" + pair.second);  // 拼接 "key=value"
    }

    return result;
}

// 根据平台返回特定的错误信息
std::string platformSupports(const std::string& command) {
#ifdef __linux__
    // 如果不在 Windows 平台上，返回空错误
    return "";
#endif
    // Windows 平台上的不支持的命令
    if (command == "user" || command == "stopsignal" || command == "arg") {
        std::string errorMessage = "The daemon on this platform does not support the command '" + command + "'";
        return errorMessage;
    }
    return "";
}

bool hasEnvName(std::vector<std::string> env, std::string name) {
    for (auto e : env) {
        if(hasPrefix(e, name + "=")) {
            return true;
        }
    }
    return false;
}
// 处理 JSON 参数的函数
std::vector<std::string> handleJSONArgs(const std::vector<std::string>& args, const std::map<std::string, bool>& attributes) {
    std::vector<std::string> result;

    if (args.empty()) {
        return result; // 如果参数为空，返回空向量
    }

    auto it = attributes.find("json");
    if (it != attributes.end() && it->second) {
        return args; // 如果属性中有 "json" 且为 true，返回原始参数
    }

    // 合并参数为一个字符串
    std::ostringstream oss;
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) {
            oss << " "; // 参数之间用空格分隔
        }
        oss << args[i];
    }

    result.push_back(oss.str()); // 将合并后的字符串放入结果向量
    return result;
}
bool hasSlash(std::string input) {
    return hasSuffix(input, "/") || hasSuffix(input, "/.");
}

// 确保提供的路径是绝对路径
std::string makeAbsolute(const std::string& dest, const std::string& workingDir) {
    std::string resultDest = dest;
    std::string resultWorkingDir = workingDir;

    // 当路径是相对路径时，调整目标路径 - 意思是，将其相对 WORKINGDIR 处理
    if (resultDest == ".") {
        if (resultWorkingDir.find(PATH_SEPARATOR) == std::string::npos) {
            resultWorkingDir += PATH_SEPARATOR;
        }
        resultDest = resultWorkingDir;
    }

    // 如果路径不是绝对路径
    if (!isPathabsolute(resultDest)) {
        bool hasSlash = (resultDest.find(PATH_SEPARATOR) != std::string::npos);

        // 使用 boost::filesystem::path 拼接路径，传入字符串形式的路径分隔符
        resultDest = (boost::filesystem::path(resultWorkingDir) / resultDest).string();

        // 确保保留末尾的斜杠
        if (hasSlash) {
            resultDest += PATH_SEPARATOR;
        }
    }
    return resultDest;
}
// // 将 map<string, string> 转换为字符串的 vector，其中键和值通过 "=" 连接
// std::vector<std::string> envMapAsSlice(const std::map<std::string, std::string>& m) {
//     std::vector<std::string> s;
//     s.reserve(m.size()); // 提前分配内存，避免多次分配

//     for (const auto& pair : m) {
//         s.push_back(pair.first + "=" + pair.second);
//     }

//     return s;
// }
// 解析选项间隔时间，返回其持续时间，如果为空则返回 0。如果值是负数，则报告错误。
std::chrono::milliseconds parseOptInterval(const std::string& flagValue, const std::string& flagName) {
    if (flagValue.empty()) {
        return std::chrono::milliseconds(0);
    }

    std::chrono::milliseconds duration;
    std::istringstream iss(flagValue);
    char unit;
    long long value;

    if (!(iss >> value >> unit)) {
        logger->log_error("Invalid duration format");
        throw std::invalid_argument("Invalid duration format");
    }

    switch (unit) {
        case 's': // seconds
            duration = std::chrono::milliseconds(value * 1000);
            break;
        case 'm': // minutes
            duration = std::chrono::milliseconds(value * 1000 * 60);
            break;
        case 'h': // hours
            duration = std::chrono::milliseconds(value * 1000 * 60 * 60);
            break;
        case 'd': // days
            duration = std::chrono::milliseconds(value * 1000 * 60 * 60 * 24);
            break;
        default:
            logger->log_error("Unsupported duration unit");
            throw std::invalid_argument("Unsupported duration unit");
    }

    if (duration.count() <= 0) {
        logger->log_error("Interval " + flagName + " must be positive");
        throw std::invalid_argument("Interval " + flagName + " must be positive");
    }

    return duration;
}