#if !defined(BUILDAH_DIGEST_H)
#define BUILDAH_DIGEST_H
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>
class digester{
    virtual ~digester() {}

    // 写入内容到摘要计算器
    virtual void write(const std::string& data) = 0;

    // 关闭摘要计算器
    virtual void close() = 0;

    // 获取内容类型
    virtual std::string ContentType() const = 0;

    // 获取摘要
    virtual std::string Digest() const = 0;
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
    std::vector<digester> digesters;
};
#endif // BUILDAH_DIGEST_H)
