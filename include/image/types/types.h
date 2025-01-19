#if !defined(IMAGE_TYPES_TYPES_H)
#define IMAGE_TYPES_TYPES_H
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <chrono>
#include "image/types/define/types.h"
#include "image/types/reference/reference.h"
#include "storage/storage/storage_reference.h"
// class ImageTransport_interface;
class Image_interface;
class ImageSource_interface;
class ImageDestination_interface;
class SystemContext;
class BlobInfo;

enum class layerCompression:int{
    PreserveOriginal=0,
    Decompress,
    Compress
};
class LayerCompression{
    public:
    layerCompression layer=layerCompression::PreserveOriginal;
    LayerCompression()=default;
    LayerCompression(layerCompression layer):layer(layer){};
};
class ImageReference_interface;
class ImageSource_interface{
    public:
    virtual ~ImageSource_interface()=default;
    virtual std::shared_ptr<ImageReference_interface> Reference() = 0;
    virtual std::tuple<std::vector<uint8_t>,std::string> GetManifest(std::shared_ptr<Digest> instanceDigest)=0;
    virtual std::vector<BlobInfo> LayerInfos()=0;
};

// ImageInspectLayer 结构体定义
class ImageInspectLayer {
    public:
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
class ImageInspectInfo {
    public:
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

class UnparsedImage_interface{
    public:
    virtual ~UnparsedImage_interface()=default;
    virtual std::tuple<std::vector<uint8_t>,std::string> Manifest()=0;
};
class Image_interface:public UnparsedImage_interface{
    public:
    virtual ~Image_interface()=default;
    virtual std::shared_ptr<ImageInspectInfo> Inspect() = 0;
    virtual std::vector<uint8_t> ConfigBlob() = 0;
};

class ImageDestination_interface{
    public:
    virtual ~ImageDestination_interface()=default;
};
class storageImageMetadata{
    public:
    std::vector<int>SignatureSizes;
    std::map<Digest, std::vector<int>> SignaturesSizes;
    storageImageMetadata()=default;
};
class storageReference;
class storageImageDestination:public ImageDestination_interface{
    public:
    std::shared_ptr<storageReference> imageRef=std::make_shared<storageReference>();
    std::string directory;
    std::vector<uint8_t> manifest;
    std::shared_ptr<Digest> manifestDigest=std::make_shared<Digest>();
    std::vector<std::shared_ptr<Digest>> untrustedDiffIDValues;
    std::vector<uint8_t> signatures;
    std::map<Digest, std::vector<uint8_t>> signatureses;
    std::shared_ptr<storageImageMetadata> metadata=std::make_shared<storageImageMetadata>();
    std::map<int,std::string>indexToStorageID;
    std::mutex lock;

    storageImageDestination()=default;
};

class ManifestUpdateInformation{
    public:
    std::shared_ptr<ImageDestination_interface> Destination=nullptr;
    std::vector<BlobInfo> LayerInfos;
    std::vector<Digest> LayerDiffIDs;
    ManifestUpdateInformation()=default;
};
class ManifestUpdateOptions{
    public:
    std::vector<BlobInfo> LayerInfos;
    std::shared_ptr<Named_interface> EmbeddedDockerReference;
    std::string ManifestMIMEType;
    std::shared_ptr<ManifestUpdateInformation> InformationOnly=std::make_shared<ManifestUpdateInformation>();
};

#endif // TYPES_TYPES_H
