#if !defined(BUILDAH_BUILAH_H)
#define BUILDAH_BUILAH_H
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "storage/storage.h"
#include "v1/config.h"
#include "buildah/digest.h"
#include "define/types_unix.h"
struct Builder {
    std::shared_ptr<store> store=nullptr;

    // Logger is the logrus logger to write log messages with
    // Logger* logger; // Pointer to Logger

    // Args 定义用户在构建时可以传递给构建器的变量。
    std::map<std::string, std::string> Args;

    // Type 用于帮助识别构建容器的元数据，不应修改。
    std::string Type;

    // FromImage 是创建容器时使用的源镜像的名称，不应修改。
    std::string FromImage;

    // FromImageID 是创建容器时使用的源镜像的 ID，不应修改。
    std::string FromImageID;

    // FromImageDigest 是创建容器时使用的源镜像的 digest，不应修改。
    std::string FromImageDigest;

    // Config 是源镜像的配置，不应修改。
    std::vector<uint8_t> Config;

    // Manifest 是源镜像的 manifest，不应修改。
    std::vector<uint8_t> Manifest;

    // Container 是构建容器的名称，不应修改。
    std::string Container;

    // ContainerID 是构建容器的 ID，不应修改。
    std::string ContainerID;

    // MountPoint 是容器的根文件系统最后一次挂载的位置，不应修改。
    std::string MountPoint;

    // ProcessLabel 是在后续 Run() 调用期间使用的 SELinux 进程标签。
    std::string ProcessLabel;

    // MountLabel 是与容器关联的 SELinux 挂载标签。
    std::string MountLabel;

    // ImageAnnotations 是存储在镜像 manifest 中的键值对。
    std::map<std::string, std::string> ImageAnnotations;

    // ImageCreatedBy 是描述如何构建此容器的说明。
    std::string ImageCreatedBy;

    // ImageHistoryComment 是描述我们添加的层如何构建的说明。
    std::string ImageHistoryComment;

    // 镜像元数据和运行时设置，支持多种格式。
    std::shared_ptr<Image> OCIv1=nullptr;
    // docker::V2Image Docker;

    // DefaultMountsFilePath 是保存挂载点的文件路径，以 "host-path:container-path" 格式。
    std::string DefaultMountsFilePath;

    // Isolation 控制我们如何处理 "RUN" 语句和 Run() 方法。
    std::shared_ptr<Isolation> Isolation=nullptr;

    // NamespaceOptions 控制如何设置我们 Run() 的进程的命名空间。
    std::vector<NamespaceOption> Namespaceoptions;

    // ConfigureNetwork 控制是否配置新的网络命名空间的网络接口和路由。
    std::shared_ptr<NetworkConfigurationPolicy> ConfigureNetwork=nullptr;

    // CNIPluginPath 是 CNI 插件助手的位置，如果它们应该从默认位置以外的地方运行。
    std::string CNIPluginPath;

    // CNIConfigDir 是 CNI 配置文件的位置，如果不使用默认配置目录中的文件。
    std::string CNIConfigDir;

    // NetworkInterface 是用于设置 CNI 或 netavark 网络的 libnetwork 网络接口。
    std::shared_ptr<ContainerNetwork> NetworkInterface=nullptr;

    // GroupAdd 是在调用 Run() 时添加到主进程的组列表。魔法值 'keep-groups' 表示进程应继承当前的补充组集。
    std::vector<std::string> GroupAdd;

    // IDMappingOptions 用于在运行进程时的非主机用户命名空间。
    std::shared_ptr<IDMappingOptions > IDMappingoptions=nullptr;

    // Capabilities 是在容器中运行命令时使用的能力列表。
    std::vector<std::string> Capabilities;

    // PrependedEmptyLayers 是在提交镜像之前，我们将添加到提交镜像的历史条目，继承自基础镜像的历史项之后。
    std::vector<History> PrependedEmptyLayers;

    // AppendedEmptyLayers 是在提交镜像之后，我们将添加到提交镜像的历史条目。
    std::vector<History> AppendedEmptyLayers;

    // CommonBuildOpts 是常见构建选项。
    std::shared_ptr<CommonBuildOptions> CommonBuildOpts=nullptr;

    // TopLayer 是镜像的顶层。
    std::string TopLayer;

    // Format 是我们最终提交的容器镜像的格式。
    std::string Format;

    // TempVolumes 是在 Run() 调用期间创建的临时挂载点。
    std::map<std::string, bool> TempVolumes;

    // ContentDigester 计算自上次重启以来所有 Add() 内容的 digest。
    std::shared_ptr<CompositeDigester> ContentDigester=nullptr;

    // Devices 是提供给 Run() 调用的额外设备。
    std::shared_ptr<ContainerDevices> Devices=nullptr;

    // DeviceSpecs 是提供给 Run() 调用的未解析的额外设备。
    std::vector<std::string> DeviceSpecs;

    // CDIConfigDir 是 CDI 配置文件的位置，如果不使用默认配置位置中的文件。
    std::string CDIConfigDir;


    void Delete();
};
#endif // BUILDAH_BUILAH_H
