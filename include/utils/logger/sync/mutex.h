#if !defined(SYNC_MUTEX_H)
#define SYNC_MUTEX_H
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

// 定义 Mutex
class Mutex {
public:
    int32_t state;
    uint32_t sema;
};
// // 定义原子操作的简单互斥量类
// class SimpleMutex {
// public:
//     SimpleMutex() : flag(ATOMIC_FLAG_INIT) {}

//     void Lock() {
//         while (flag.test_and_set(std::memory_order_acquire)) {
//             // 自旋等待
//         }
//     }

//     void Unlock() {
//         flag.clear(std::memory_order_release);
//     }

// public:
//     std::atomic_flag flag;
// };

#endif // SYNC_MUTEX_H