#if !defined(UTILS_LOGGER_LOGRUS_LOGRUS_H)
#define UTILS_LOGGER_LOGRUS_LOGRUS_H
#include <string>
#include <vector>
#include "utils/common/error.h"
#include <algorithm>
#include <boost/any.hpp>
enum class level : uint32_t {
    PanicLevel=0,
    FatalLevel,
    ErrorLevel,
    WarnLevel,
    InfoLevel,
    DebugLevel,
    TraceLevel
};
class Level {
public:
    level lvl;

    Level() = default;
    Level(level lvl) : lvl(lvl) {};

    std::string String();
    std::vector<uint8_t> MarshalText();
    void UnmarshalText(std::vector<uint8_t> text);

    // 重载比较运算符
    bool operator>=(const Level& other) const {
        return static_cast<uint32_t>(lvl) >= static_cast<uint32_t>(other.lvl);
    }
    bool operator==(const Level& other) const {
        return static_cast<uint32_t>(lvl) == static_cast<uint32_t>(other.lvl);
    }
    bool operator<=(const Level& other) const {
        return static_cast<uint32_t>(lvl) <= static_cast<uint32_t>(other.lvl);
    }
    bool operator<(const Level& other) const {
        return static_cast<uint32_t>(lvl) < static_cast<uint32_t>(other.lvl);
    }
};

Level ParseLevel(std::string lvl);
#endif // LOGRUS_LOGRUS_H
