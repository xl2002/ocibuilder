#if !defined(LOGRUS_HOOKS_H)
#define LOGRUS_HOOKS_H
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include "logrus/logrus.h"
#include "logrus/entry.h"

//using LevelHooks = std::unordered_map<Level, std::vector<Hook>>;
class Hook{
public:
    virtual ~Hook() = default;
    //返回Hook应用的日志级别
    virtual std::vector<Level> Levels() = 0;
    virtual int Fire(std::shared_ptr<Entry> entry)=0;
    
};
//int Fire(LevelHooks& hooks, const Level& level, std::shared_ptr<Entry> entry);

class LevelHooks {
private:
    std::map<Level, std::vector<std::shared_ptr<Hook>>> hooks;

public:
    void Add(const std::shared_ptr<Hook>& hook);
    void Fire(Level level, std::shared_ptr<Entry> entry);
};
#endif // LOGRUS_HOOKS_H

