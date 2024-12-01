#if !defined(LOGRUS_HOOKS_H)
#define LOGRUS_HOOKS_H
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include "utils/logger/logrus/logrus.h"
#include "utils/logger/logrus/entry.h"
class Entry;
//using LevelHooks = std::unordered_map<Level, std::vector<Hook_interface>>;
class Hook_interface {
public:
    virtual ~Hook_interface() = default;
    //返回Hook应用的日志级别
    virtual std::vector<Level> Levels() = 0;
    virtual int Fire(std::shared_ptr<Entry> entry)=0;
    
};
//int Fire(LevelHooks& hooks, const Level& level, std::shared_ptr<Entry> entry);

class LevelHooks {
public:
    std::map<Level, std::vector<std::shared_ptr<Hook_interface>>> hooks;
    

public:
    LevelHooks()=default;
    void Add(const std::shared_ptr<Hook_interface>& hook);
    void Fire(Level level, std::shared_ptr<Entry> entry);
};
#endif // LOGRUS_HOOKS_H

