#if !defined(IO_IO_H)
#define IO_IO_H
#include <string>
#include <iostream>
#include <vector>
#include <memory>

// 定义 Writer 接口
class Writer {
public:
    virtual std::pair<int, std::string> Write(const std::vector<uint8_t>& p) = 0;
    virtual ~Writer() = default;
};
// 定义 Reader 接口
class Reader {
    public:
    virtual std::pair<int, std::string> Readere(const std::vector<uint8_t>& p) = 0;
    virtual ~Reader() = default;
};
// 定义 Closer 接口
class Closer {
public:
    virtual ~Closer() = default;
    virtual std::error_code Close() = 0;
};
// 定义 ReadCloser 接口，继承自 Reader 和 Closer
class ReadCloser : public Reader, public Closer {
};
#endif // IO_IO_H