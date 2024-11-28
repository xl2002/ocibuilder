#if !defined(IMAGE_BUILDAH_DIGEST_H)
#define IMAGE_BUILDAH_DIGEST_H
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "image/digest/digest.h"
class digester_interface{
    public:
    virtual ~digester_interface()=default;

    // 写入内容到摘要计算器
    virtual void write(const std::string& data) = 0;

    // 关闭摘要计算器
    virtual void close() = 0;

    // 获取内容类型
    virtual std::string ContentType() const = 0;

    // 获取摘要
    virtual std::shared_ptr<Digest> Digester() const = 0;
};
class SimpleHash {//hash函数,标准库没有

};
// simpleDigester 类
// class simpleDigester : public digester {
// public:
//     std::shared_ptr<SimpleHash> hasher_;
//     std::string contentType_;
// };
class simpleDigester {

};
class CompositeDigester{
    public:
    std::vector<std::shared_ptr<digester_interface>> digesters; // 存储 Digester 对象
    std::shared_ptr<std::ofstream> closer; // 存储 Closable 对象

    void closeOpenDigester();
    std::pair<std::string, std::shared_ptr<Digest>> Digest();
};
#endif // BUILDAH_DIGEST_H)
