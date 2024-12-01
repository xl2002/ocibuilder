#if !defined(LOGRUS_TEXT_FORMATTER_H)
#define LOGRUS_TEXT_FORMATTER_H
#include <string>
#include <vector>
#include <functional>
#include "runtime/runtime2.h"
#include "utils/logger/logrus/json_formatter.h"
#include "utils/logger/logrus/formatter.h"
#include "utils/logger/sync/once.h"
struct TextFormatter : public Formatter_interface{
public:
    bool ForceColors = false;             // 强制输出颜色
    bool DisableColors = false;           // 禁用颜色
    bool ForceQuote = false;              // 强制引用所有值
    bool DisableQuote = false;            // 禁用引用值
    bool EnvironmentOverrideColors = false; // 基于环境变量覆盖颜色设置
    bool DisableTimestamp = false;        // 禁用时间戳
    bool FullTimestamp = false;           // 完整时间戳
    std::string TimestampFormat;          // 时间戳格式
    bool DisableSorting = false;          // 禁用字段排序
    std::function<void(std::vector<std::string>&)> SortingFunc; // 字段排序函数
    bool DisableLevelTruncation = false;  // 禁用级别文本截断
    bool PadLevelText = false;            // 为级别文本添加填充
    bool QuoteEmptyFields = false;        // 对空字段加引号
    bool isTerminal = false;              // 是否输出到终端
    FieldMap fieldMap;                    // 自定义字段名称的映射
    std::function<std::pair<std::string, std::string>(Frame*)> CallerPrettyfier; // 自定义函数和文件名

    once terminalInitOnce;      // 保证某些操作只执行一次
    int levelTextMaxLength = 0;           // 级别文本的最大长度，初始化时动态生成

    // 实现 Formatter 接口的纯虚函数
    std::pair<std::vector<uint8_t>, std::string> Format(void* entry) override {
        // 实现具体的格式化逻辑
        return { {}, "formatted log" };
    }
};
#endif // LOGRUS_TEXT_FORMATTER_H