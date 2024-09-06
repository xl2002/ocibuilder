#include "imagebuildah/util.h"
std::vector<std::string> argsMapToSlice(std::map<std::string, std::string> m) {
    std::vector<std::string> s;
    s.reserve(m.size()); // 预分配空间以提高性能
    for (const auto& pair : m) {
        s.push_back(pair.first + "=" + pair.second);
    }
    return s;
}