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
};


#endif // BYTES_BUFFER_H