#if !defined(TYPES_TYPES_H)
#define TYPES_TYPES_H
#include <string>
#include <memory>
#include <vector>
#include <map>
#include "reference/reference.h"
class ImageTransport_interface;
class ImageReference_interface{
    public:
    ~ImageReference_interface()=default;
    virtual std::shared_ptr<ImageTransport_interface> Transport() = 0;
    virtual std::string StringWithinTransport() = 0;
    virtual std::string PolicyConfigurationIdentity() = 0;
    virtual std::vector<std::string> PolicyConfigurationNamespaces() = 0;
    virtual std::shared_ptr<Named_interface> DockerReference() = 0;
};
// ImageTransport基类
class ImageTransport_interface {
public:
    virtual ~ImageTransport_interface() {}

    // 返回传输名称
    virtual std::string Name()  = 0;

    // 将字符串转换为ImageReference
    virtual std::shared_ptr<ImageReference_interface> ParseReference(const std::string &reference)  = 0;

    // 验证策略配置范围
    virtual void ValidatePolicyConfigurationScope(const std::string &scope)  = 0;
};


#endif // TYPES_TYPES_H
