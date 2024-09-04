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
};

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
