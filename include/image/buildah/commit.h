#if !defined(IMAGE_BUILDE_COMMIT_H)
#define IMAGE_BUILDE_COMMIT_H
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>    
#include <boost/optional.hpp>
#include <iostream>  // for std::ostream
#include "image/types/define/build.h"
#include "image/types/define/types.h"
#include "config/config.h"
#include "image/image_types/manifest/docker_schema2.h"
struct CommitOptions {
    // PreferredManifestType 是图片清单的首选类型
    std::string PreferredManifestType;

    // Compression 指定对层 blob 应用的压缩类型
    std::shared_ptr<Compression> Compression=std::make_shared<::Compression>();  // 这里使用 std::string 表示压缩类型

    // SignaturePolicyPath 指定用于验证新图片的签名策略位置
    std::string SignaturePolicyPath;

    // AdditionalTags 是要添加到图片的额外名称列表
    std::vector<std::string> AdditionalTags;

    // ReportWriter 是一个 io.Writer，将用于记录新图片的写入日志
    std::ostream* ReportWriter=nullptr;

    // HistoryTimestamp 是创建图片历史项目时使用的时间戳
    std::shared_ptr<std::chrono::system_clock::time_point> HistoryTimestamp=std::make_shared<std::chrono::system_clock::time_point>();

    // SystemContext 用于持有凭据和其他身份验证信息
    std::shared_ptr<SystemContext> SystemContext=std::make_shared<::SystemContext>();  // 根据实际定义进行类型替换

    // IIDFile 告诉构建器将图片 ID 写入指定的文件
    std::string IIDFile;

    // Squash 告诉构建器创建单层图片
    bool Squash=false;

    // OmitHistory 告诉构建器在准备 image-spec 时忽略构建层和基础层的历史
    bool OmitHistory=false;

    // BlobDirectory 是包含层 blob 的预构建副本的目录名
    std::string BlobDirectory;

    // EmptyLayer 告诉构建器忽略工作容器的差异
    bool EmptyLayer=false;

    // OmitTimestamp 强制将创建时间戳设为 epoch 0，以实现可确定性、内容地址的构建
    bool OmitTimestamp=false;

    // SignBy 是用于签署图片的 GPG 密钥指纹
    std::string SignBy;

    // Manifest 列表将图片添加到其中
    std::string Manifest;

    // MaxRetries 是将图片提交到外部注册表时的最大尝试次数
    int MaxRetries=0;

    // RetryDelay 是重试提交图片的延迟时间
    std::chrono::duration<int> RetryDelay;

    // OciEncryptConfig 指示是否加密图片
    std::shared_ptr<EncryptConfig> OciEncryptConfig=std::make_shared<::EncryptConfig>();  // 根据实际定义进行类型替换

    // OciEncryptLayers 表示要加密的层的列表
    std::vector<int> OciEncryptLayers;

    // ConfidentialWorkloadOptions 强制将图片的 rootfs 内容替换为 LUKS 兼容的加密磁盘映像
    std::shared_ptr<ConfidentialWorkloadOptions> ConfidentialWorkloadOptions=std::make_shared<::ConfidentialWorkloadOptions>();

    // UnsetEnvs 是不要添加到最终图片中的环境变量列表
    std::vector<std::string> UnsetEnvs;

    // OverrideConfig 是一个可选的 Schema2Config，用于覆盖工作容器的配置
    std::shared_ptr<Schema2Config> OverrideConfig=std::make_shared<::Schema2Config>();  // 根据实际定义进行类型替换

    // OverrideChanges 是对图片配置的 Dockerfile 风格指令的列表
    std::vector<std::string> OverrideChanges;

    // ExtraImageContent 是一个描述要添加到提交图片的额外内容的映射
    std::map<std::string, std::string> ExtraImageContent;

    // SBOMScanOptions 包含控制是否对即将提交的 rootfs 运行扫描器以及如何运行的选项
    std::vector<SBOMScanOptions> SBOMScanOptions;
};

#endif // BUILDE_COMMIT_H
