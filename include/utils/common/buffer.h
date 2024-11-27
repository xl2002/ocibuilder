#if !defined(BYTES_BUFFER_H)
#define BYTES_BUFFER_H
#include <string>
#include <iostream>
#include <vector>
#include <memory>

// 定义 byte 类型
using byte = uint8_t;
// 定义 readOp 类型
enum class readOp : int8_t {
    NONE,
    READ,
    UNREAD
};
// 定义 Buffer 类
class Buffer {
public:
    std::vector<byte> buf; // contents are the bytes buf[off : len(buf)]
    size_t off = 0;        // read at &buf[off], write at &buf[len(buf)]
    readOp lastRead = readOp::NONE; // last read operation

    Buffer() = default;

    // Reset 方法将缓冲区重置为空状态，但保留底层存储
    void Reset() {
        if(buf.size() > 0) {
            buf.clear();
        }
        // buf.clear();            // 清空缓冲区但保留内存
        off = 0;                // 重置偏移量
        lastRead = readOp::NONE; // 重置最后的读操作
    }
};


#endif // BYTES_BUFFER_H