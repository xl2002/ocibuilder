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

    Pointer local = nullptr;   // 用 Pointer 模拟 Go 的 Pointer
    size_t localSize = 0;      // 用 size_t 模拟 uintptr

    Pointer victim = nullptr;  // 用 Pointer 模拟 Go 的 Pointer
    size_t victimSize = 0;     // 用 size_t 模拟 uintptr

    // 用 std::function<void()> 模拟 func() any
    std::function<void()> New;

    Pool() = default;
};

#endif // SYNC_POOL_H