#if !defined(SYNC_POOL_H)
#define SYNC_POOL_H
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include "sync/cond.h"
#include "logrus/entry.h"
// 定义 ArbitraryType 类型
// using ArbitraryType = int;

// 定义 std::shared_ptr<int> 类型
// using std::shared_ptr<int>=std::shared_ptr<int> ;
// typedef std::shared_ptr<int>=int*;

// 定义 Pool 类
class Entry;
class Pool {
public:
    noCopy noCopy;

    std::shared_ptr<int> local = std::make_shared<int>();   
    size_t localSize = 0;      

    std::shared_ptr<int> victim = std::make_shared<int>();  
    size_t victimSize = 0;     

    
    std::function<void()> New;

    Pool() = default;

    bool empty() const {
        return pool.empty();
    }

    std::shared_ptr<Entry> front() {
        if (!pool.empty()) {
            return pool.back();
        }
        return nullptr;
    }

    void pop() {
        if (!pool.empty()) {
            pool.pop_back();
        }
    }

    void push(std::shared_ptr<Entry> entry) {
        pool.push_back(entry);
    }

public:
    std::vector<std::shared_ptr<Entry>> pool;  // 使用 vector 存储 Entry 对象的指针
};


#endif // SYNC_POOL_H