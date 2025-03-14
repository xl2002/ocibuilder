#if !defined(UTILS_LOGGER_LOGRUS_FORMATTER_H)
#define UTILS_LOGGER_LOGRUS_FORMATTER_H
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

// 定义 Formatter_interface 接口
class Formatter_interface {
public:
    virtual std::pair<std::vector<uint8_t>, std::string> Format(void* entry) = 0;
    ~Formatter_interface() = default;
};

#endif // LOGRUS_FORMATTER_H