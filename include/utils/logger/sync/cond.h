#if !defined(SYNC_COND_H)
#define SYNC_COND_H
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

// 定义 noCopy 结构体
// noCopy 可以添加到那些在第一次使用后不能被复制的结构体中。
// 不能被拷贝或赋值
// 请注意，由于 Lock 和 Unlock 方法，它不能被嵌入。
class noCopy {
    public:
    noCopy() = default;
    noCopy(const noCopy&) = delete;
    noCopy& operator=(const noCopy&) = delete;
};


#endif // SYNC_COND_H