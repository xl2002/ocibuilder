#if !defined(IMAGE_BUILDAH_IMAGE_H)
#define IMAGE_BUILDAH_IMAGE_H
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>
#include <chrono>
#include "storage/storage/storage.h"
#include "image/types/define/build.h"
#include "image/types/define/types.h"
#include "utils/common/buffer.h"
#include "image/image_types/v1/config.h"
#include "image/image_types/manifest/docker_schema2.h"
#include "image/image_types/v1/config.h"
#include "image/image_types/v1/oci.h"
// struct v1::Image;
struct containerImageRef;
// 容器镜像引用的结构体定义
struct containerImageRef: public ImageReference_interface{
    std::string fromImageName;                  // 源镜像的名称
    std::string fromImageID;                    // 源镜像的ID
    std::shared_ptr<Store_interface> store=nullptr;                        // 存储库
    std::shared_ptr<Compression> compression=std::make_shared<::Compression>();             // 压缩类型
    std::string name;                           // 镜像名称
    std::vector<std::string> names;             // 镜像的别名
    std::string containerID;                    // 容器的ID
    std::string mountLabel;                     // 挂载标签
    std::string layerID;                        // 层的ID
    std::vector<byte> oconfig;                  // 原始配置（字节数组）
    std::vector<byte> dconfig;                  // 目标配置（字节数组）
    std::chrono::system_clock::time_point created;                           // 创建时间
    std::string createdBy;                      // 创建者
    std::string historyComment;                 // 历史记录注释
    std::map<std::string, std::string> annotations; // 注解
    std::string preferredManifestType;          // 首选的镜像清单类型
    bool squash=false;                                // 是否压缩成单层镜像
    std::shared_ptr<ConfidentialWorkloadOptions> confidentialWorkload=std::make_shared<ConfidentialWorkloadOptions>(); // 机密工作负载选项
    bool omitHistory=false;                           // 是否忽略历史记录
    bool emptyLayer=false;                            // 是否是空层
    std::shared_ptr<IDMappingOptions> idMappingOptions=std::make_shared<IDMappingOptions>();         // ID映射选项
    std::string parent;                         // 父镜像ID
    std::string blobDirectory;                  // blob 存放目录
    std::vector<History> preEmptyLayers;    // 空层（操作前）
    std::vector<History> postEmptyLayers;   // 空层（操作后）
    std::vector<std::string> overrideChanges;   // 覆盖配置的更改
    std::shared_ptr<Schema2Config> overrideConfig=std::make_shared<Schema2Config>();              // 覆盖配置
    std::map<std::string, std::string> extraImageContent; // 额外的镜像内容
    containerImageRef()=default;
    std::shared_ptr<ImageTransport_interface> Transport() override;
    std::string StringWithinTransport() override;
    std::string PolicyConfigurationIdentity() override;
    std::vector<std::string> PolicyConfigurationNamespaces() override;
    std::shared_ptr<Named_interface> DockerReference() override;
    std::shared_ptr<Image_interface> NewImage(std::shared_ptr<SystemContext>sys) override;
    std::shared_ptr<ImageSource_interface> NewImageSource(std::shared_ptr<SystemContext>sys) override;
    std::tuple<std::shared_ptr<v1::Image>,std::shared_ptr<Manifest>> createConfigsAndManifests();
    std::shared_ptr<ImageDestination_interface> NewImageDestination(std::shared_ptr<SystemContext>sys) override;
};
struct blobLayerInfo{
    std::string ID;
    int64_t Size=0;
    blobLayerInfo()=default;
};

std::string computeLayerMIMEType(std::string what,std::shared_ptr<Compression> layerCompression);
struct containerImageSource: public ImageSource_interface{
    std::string path;                          // Go: string
    std::shared_ptr<containerImageRef> ref=std::make_shared<containerImageRef>();                    // Go: *containerImageRef
    std::shared_ptr<Store_interface> store=nullptr;                        // Go: storage.Store
    std::string containerID;                   // Go: string
    std::string mountLabel;                    // Go: string
    std::string layerID;                       // Go: string
    std::vector<std::string> names;            // Go: []string
    std::shared_ptr<Compression> compression=std::make_shared<::Compression>();           // Go: archive.Compression
    std::vector<uint8_t> config;         // Go: []byte
    std::shared_ptr<Digest> configDigest=std::make_shared<Digest>();                 // Go: digest.Digest
    std::vector<uint8_t> manifest;       // Go: []byte
    std::string manifestType;                  // Go: string
    std::string blobDirectory;                 // Go: string
    std::map<Digest, blobLayerInfo> blobLayers; // Go: map[digest.Digest]blobLayerInfo
    std::shared_ptr<ImageReference_interface> Reference()override;
    std::tuple<std::vector<uint8_t>,std::string> GetManifest(std::shared_ptr<Digest> instanceDigest) override;

    std::vector<BlobInfo> LayerInfos()override;
    std::shared_ptr<OCI1>OCI1FromManifest();
};
#endif // BUILDAH_IMAGE_H)
