#if !defined(LOGRUS_FORMATTER_H)
#define LOGRUS_FORMATTER_H
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

// 定义 Formatter 接口
class Formatter {
public:
    virtual std::pair<std::vector<uint8_t>, std::string> Format(void* entry) = 0;
    virtual ~Formatter() = default;
};

#endif // LOGRUS_FORMATTER_H