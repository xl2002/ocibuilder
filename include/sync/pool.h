#if !defined(SYNC_POOL_H)
#define SYNC_POOL_H
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include "sync/cond.h"

// 定义 ArbitraryType 类型
using ArbitraryType = int;

// 定义 Pointer 类型
using Pointer = ArbitraryType*;

// 定义 Pool 类
class Pool {
public:
    noCopy noCopy;

    Pointer local = nullptr;   
    size_t localSize = 0;      

    Pointer victim = nullptr;  
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

private:
    std::vector<std::shared_ptr<Entry>> pool;  // 使用 vector 存储 Entry 对象的指针
};


#endif // SYNC_POOL_H