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
    virtual std::string ContentType() = 0;

    // 获取摘要
    virtual std::shared_ptr<::Digest> Digest() = 0;
};
class SimpleHash {//hash函数,标准库没有

};
/**
 * @brief 一个简单的digester，只是按原样消化其内容。
 * 
 */
class simpleDigester:public digester_interface{
    public:
    std::shared_ptr<Digester_interface> digester;
    std::shared_ptr<Hash_256> hasher;
    std::string contentType;
    std::string ContentType() override;
    void write(const std::string& data) override;
    void close() override;
    std::shared_ptr<::Digest> Digest() override;
};
std::shared_ptr<digester_interface> newSimpleDigester(string contentType);
class CompositeDigester{
    public:
    std::vector<std::shared_ptr<digester_interface>> digesters; // 存储 Digester 对象
    // std::ofstream closer; // 存储 Closable 对象
    std::shared_ptr<digester_interface> closer;
    void closeOpenDigester();
    std::pair<std::string, std::shared_ptr<Digest>> Digest();
    void Restart();
    void Start(std::string contentType);
    std::shared_ptr<digester_interface> Hash();
};

#endif // BUILDAH_DIGEST_H)
