#if !defined(TYPES_TYPES_H)
#define TYPES_TYPES_H
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include "reference/reference.h"
class ImageTransport_interface;
class ImageReference_interface{
    public:
    virtual ~ImageReference_interface()=default;
    virtual std::shared_ptr<ImageTransport_interface> Transport() = 0;
    virtual std::string StringWithinTransport() = 0;
    virtual std::string PolicyConfigurationIdentity() = 0;
    virtual std::vector<std::string> PolicyConfigurationNamespaces() = 0;
    virtual std::shared_ptr<Named_interface> DockerReference() = 0;
};
// ImageTransport基类
class ImageTransport_interface {
public:
    virtual ~ImageTransport_interface()=default;

    // 返回传输名称
    virtual std::string Name()  = 0;

    // 将字符串转换为ImageReference
    virtual std::shared_ptr<ImageReference_interface> ParseReference(const std::string &reference)  = 0;

    // 验证策略配置范围
    virtual void ValidatePolicyConfigurationScope(const std::string &scope)  = 0;
};

enum class layerCompression:int{
    PreserveOriginal=0,
    Decompress,
    Compress
};
struct LayerCompression{
    layerCompression layer=layerCompression::PreserveOriginal;
    LayerCompression()=default;
    LayerCompression(layerCompression layer):layer(layer){};
};

struct ImageSource_interface{
    virtual std::shared_ptr<ImageReference_interface> Reference() = 0;
};
// ImageInspectLayer 结构体定义
struct ImageInspectLayer {
    // MIME 类型，未知时为空字符串
    std::string MIMEType;

    // 摘要
    std::shared_ptr<::Digest> Digest=std::make_shared<::Digest>(); // 需要根据实际情况定义 Digest

    // 大小，未知时为 -1
    int64_t Size=-1; 

    // 注释映射
    std::map<std::string, std::string> Annotations;

};
// ImageInspectInfo 结构体定义
struct ImageInspectInfo {
    // 标签字段是一个遗留字段，仅用于 Docker v2s1 清单。不会支持其他清单类型。
    std::string Tag;
    
    // 创建时间，使用 std::shared_ptr 来管理动态内存
    std::shared_ptr<std::chrono::system_clock::time_point> Created;

    // Docker 版本
    std::string DockerVersion;

    // 标签映射
    std::map<std::string, std::string> Labels;

    // 架构
    std::string Architecture;

    // 变种
    std::string Variant;

    // 操作系统
    std::string Os;

    // 层
    std::vector<std::string> Layers;

    // 层数据
    std::vector<ImageInspectLayer> LayersData; // 需要根据实际情况定义 ImageInspectLayer

    // 环境变量
    std::vector<std::string> Env;

    // 作者
    std::string Author;

};
#endif // TYPES_TYPES_H
