#if !defined(BUILDAH_IMAGE_H)
#define BUILDAH_IMAGE_H
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>
#include <chrono>
#include "storage/storage.h"
#include "define/build.h"
#include "define/types.h"
#include "bytes/buffer.h"
#include "v1/config.h"
#include "manifest/docker_schema2.h"
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

    std::shared_ptr<ImageTransport_interface> Transport() override;
    std::string StringWithinTransport() override;
    std::string PolicyConfigurationIdentity() override;
    std::vector<std::string> PolicyConfigurationNamespaces() override;
    std::shared_ptr<Named_interface> DockerReference() override;
};

#endif // BUILDAH_IMAGE_H)
