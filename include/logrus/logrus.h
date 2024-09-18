#if !defined(LOGRUS_LOGRUS_H)
#define LOGRUS_LOGRUS_H
#include <string>
#include <vector>
#include "cobra/error.h"
#include <algorithm>
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
};

// 为 Level 类型提供哈希函数
namespace std {
    template <>
    struct hash<Level> {
        std::size_t operator()(const Level& level) const {
            return hash<uint32_t>()(static_cast<uint32_t>(level.lvl));
        }
    };
}

// std::vector<Level> AllLevels{
//     Level(level::PanicLevel),
//     Level(level::FatalLevel),
//     Level(level::ErrorLevel),
//     Level(level::WarnLevel),
//     Level(level::InfoLevel),
//     Level(level::DebugLevel),
//     Level(level::TraceLevel)
// };
Level ParseLevel(std::string lvl);
#endif // LOGRUS_LOGRUS_H
