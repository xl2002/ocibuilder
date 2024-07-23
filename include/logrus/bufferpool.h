#if !defined(LOGRUS_BUFFERPOOL_H)
#define LOGRUS_BUFFERPOOL_H
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "bytes/buffer.h"

// 定义 BufferPool 接口
class BufferPool {
public:
    virtual ~BufferPool() = default;

    virtual void Put(std::unique_ptr<Buffer> buffer) = 0;
    virtual std::unique_ptr<Buffer> Get() = 0;
};

#endif // LOGRUS_BUFFERPOOL_H