#if !defined(IMAGE_LIBIMAGE_INSPECT_H)
#define IMAGE_LIBIMAGE_INSPECT_H
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <map>
#include "image/image_types/v1/config.h"
#include "image/digest/digest.h"
#include "image/image_types/manifest/docker_schema2.h"
// #include "image/types/define/types.h"
// #include "storage/storage/storage.h"
// #include "image/libimage/image.h"
struct DriverData{
    std::string Name;
    std::map<std::string, std::string> Data;
};
namespace LibImage {
    struct RootFS{
        std::string Type;
        std::vector<Digest> Layers;
    };
}
// ImageData 结构体定义
struct ImageData {
    // 镜像 ID
    std::string ID;

    // 摘要
    std::shared_ptr<::Digest> Digest=std::make_shared<::Digest>(); // 需要根据实际情况定义 Digest

    // 镜像标签
    std::vector<std::string> RepoTags;

    // 镜像摘要
    std::vector<std::string> RepoDigests;

    // 父镜像
    std::string Parent;

    // 注释
    std::string Comment;

    // 创建时间
    std::shared_ptr<std::chrono::system_clock::time_point> Created=std::make_shared<std::chrono::system_clock::time_point>(); // 用指针表示时间

    // 镜像配置
    std::shared_ptr<ImageConfig> Config=std::make_shared<ImageConfig>(); // 指向 ImageConfig 的指针

    // 版本
    std::string Version;

    // 作者
    std::string Author;

    // 架构
    std::string Architecture;

    // 操作系统
    std::string Os;

    // 大小
    std::int64_t Size=-1;

    // 虚拟大小
    std::int64_t VirtualSize=-1;

    // 图形驱动数据
    std::shared_ptr<DriverData> GraphDriver=std::make_shared<DriverData>(); // 指向 DriverData 的指针

    // 根文件系统
    std::shared_ptr<LibImage::RootFS> RootFS=std::make_shared<LibImage::RootFS>(); // 指向 RootFS 的指针

    // 标签
    std::map<std::string, std::string> Labels;

    // 注释
    std::map<std::string, std::string> Annotations;

    // 清单类型
    std::string ManifestType;

    // 用户
    std::string User;

    // 历史记录
    std::vector<History> History; // 需要根据实际情况定义 History

    // 名称历史
    std::vector<std::string> NamesHistory;

    // 健康检查配置
    std::shared_ptr<Schema2HealthConfig> HealthCheck=std::make_shared<Schema2HealthConfig>(); // 指向 HealthConfig 的指针，允许为空
};

struct InspectOptions{
    bool WithSize=false;
    bool WithParent=false;
};

#endif // LIBIMAGE_INSPECT_H)
