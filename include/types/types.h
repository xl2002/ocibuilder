#if !defined(TYPES_TYPES_H)
#define TYPES_TYPES_H
#include <string>
#include <memory>

class ImageTransport;
class ImageReference{
    public:
    ~ImageReference()=default;
    virtual std::shared_ptr<ImageTransport> Transport() const = 0;

};
// ImageTransport基类
class ImageTransport {
public:
    virtual ~ImageTransport() {}

    // 返回传输名称
    virtual std::string Name() const = 0;

    // 将字符串转换为ImageReference
    virtual std::shared_ptr<ImageReference> ParseReference(const std::string &reference) const = 0;

    // 验证策略配置范围
    virtual void ValidatePolicyConfigurationScope(const std::string &scope) const = 0;
};

// class imageTransport : public ImageTransport {
// public:
//     virtual ~imageTransport() {}

//     // 返回传输名称 
//     std::string Name() const override {
//         return "image";
//     }

//     // 将字符串转换为ImageReference 
//     std::shared_ptr<ImageReference> ParseReference(const std::string &reference) const override {
//         return std::make_shared<ImageReference>();
//     }

//     // 验证策略配置范围
//     void ValidatePolicyConfigurationScope(const std::string &scope) const override {
//         // TODO
//     }   

// };

#endif // TYPES_TYPES_H
