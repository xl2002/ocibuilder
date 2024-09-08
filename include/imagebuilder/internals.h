#if !defined(IMAGEBUILDER_INTERNALS_H)
#define IMAGEBUILDER_INTERNALS_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <functional>
#include <tuple>
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




#endif // IMAGEBUILDER_INTERNALS_H
