#if !defined(IMAGE_BUILDAH_BUILDAH_H)
#define IMAGE_BUILDAH_BUILDAH_H
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <memory>
#include "storage/storage/storage.h"
#include "image/image_types/v1/config.h"
#include "image/buildah/digester.h"
#include "image/types/define/types_unix.h"
#include "image/image_types/docker/types.h"
#include "image/types/define/types.h"
#include "image/types/types.h"
#include "image/buildah/commit.h"
#include "image/types/define/build.h"
#include "image/types/signature/policy_config.h"
#include "image/buildah/image.h"
// #include "utils/logger/logrus/logger.h"
#include "image/buildah/add.h"
// class v1::Image;
class Store;
class Container;
class Builder {
    public:
    std::shared_ptr<Store> store=std::make_shared<Store>();

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
    std::shared_ptr<::Container> container=std::make_shared<::Container>();
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
    std::shared_ptr<v1::Image> OCIv1=std::make_shared<v1::Image>();
    std::shared_ptr<V2Image> Docker=std::make_shared<V2Image>();

    // DefaultMountsFilePath 是保存挂载点的文件路径，以 "host-path:container-path" 格式。
    std::string DefaultMountsFilePath;

    // Isolation 控制我们如何处理 "RUN" 语句和 Run() 方法。
    std::shared_ptr<::Isolation> Isolation=std::make_shared<::Isolation>();

    // NamespaceOptions 控制如何设置我们 Run() 的进程的命名空间。
    std::vector<NamespaceOption> Namespaceoptions;

    // ConfigureNetwork 控制是否配置新的网络命名空间的网络接口和路由。
    std::shared_ptr<NetworkConfigurationPolicy> ConfigureNetwork=std::make_shared<NetworkConfigurationPolicy>();

    // CNIPluginPath 是 CNI 插件助手的位置，如果它们应该从默认位置以外的地方运行。
    std::string CNIPluginPath;

    // CNIConfigDir 是 CNI 配置文件的位置，如果不使用默认配置目录中的文件。
    std::string CNIConfigDir;

    // NetworkInterface 是用于设置 CNI 或 netavark 网络的 libnetwork 网络接口。
    std::shared_ptr<ContainerNetwork> NetworkInterface=std::make_shared<ContainerNetwork>();

    // GroupAdd 是在调用 Run() 时添加到主进程的组列表。魔法值 'keep-groups' 表示进程应继承当前的补充组集。
    std::vector<std::string> GroupAdd;

    // IDMappingOptions 用于在运行进程时的非主机用户命名空间。
    std::shared_ptr<IDMappingOptions > IDMappingoptions=std::make_shared<IDMappingOptions>();

    // Capabilities 是在容器中运行命令时使用的能力列表。
    std::vector<std::string> Capabilities;

    // PrependedEmptyLayers 是在提交镜像之前，我们将添加到提交镜像的历史条目，继承自基础镜像的历史项之后。
    std::vector<History> PrependedEmptyLayers;

    // AppendedEmptyLayers 是在提交镜像之后，我们将添加到提交镜像的历史条目。
    std::vector<History> AppendedEmptyLayers;

    // CommonBuildOpts 是常见构建选项。
    std::shared_ptr<CommonBuildOptions> CommonBuildOpts=std::make_shared<CommonBuildOptions>();

    // TopLayer 是镜像的顶层。
    std::string TopLayer;

    // Format 是我们最终提交的容器镜像的格式。
    std::string Format;

    // TempVolumes 是在 Run() 调用期间创建的临时挂载点。
    std::map<std::string, bool> TempVolumes;

    // ContentDigester 计算自上次重启以来所有 Add() 内容的 digest。
    std::shared_ptr<CompositeDigester> ContentDigester=std::make_shared<CompositeDigester>();

    // Devices 是提供给 Run() 调用的额外设备。
    std::shared_ptr<ContainerDevices> Devices=std::make_shared<ContainerDevices>();

    // DeviceSpecs 是提供给 Run() 调用的未解析的额外设备。
    std::vector<std::string> DeviceSpecs;

    // CDIConfigDir 是 CDI 配置文件的位置，如果不使用默认配置位置中的文件。
    std::string CDIConfigDir;


    void Delete();
    void AddPrependedEmptyLayer(std::shared_ptr<std::chrono::system_clock::time_point> created, std::string createdBy,std::string author, std::string comment);
    void SetCreatedBy(std::string how);
    std::string CreatedBy();
    void ClearAnnotations();
    void SetMaintainer(std::string who);
    void SetArchitecture(std::string arch);
    std::string OS();
    void SetOS(std::string os);
    std::string OSVersion();
    void SetOSVersion(std::string osVersion);
    std::vector<std::string> Ports();
    void SetPorts(std::string port);
    void SetHostname(std::string name);
    void SetDomainname(std::string name);
    void SetUser(std::string spec);
    void ClearPorts();
    void SetEnv(std::string k,std::string v);
    void SetCmd(std::vector<std::string> cmd);
    void ClearVolumes();
    std::vector<std::string> Volumes();
    void AddVolume(std::string k,std::string v);
    void ClearOnBuild();
    void SetWorkDir(std::string there);
    void SetEntrypoint(std::vector<std::string> ep);
    void SetShell(std::vector<std::string> shell);
    void SetStopSignal(std::string sig);
    void SetHealthcheck(std::shared_ptr<HealthConfig> config);
    void ClearLabels();
    // std::string Architecture();
    void SetLabel(std::string k,std::string v);
    std::tuple<std::string,std::shared_ptr<Canonical_interface>,std::shared_ptr<Digest>> Commit(
        std::shared_ptr<ImageReference_interface> dest,
        std::shared_ptr<CommitOptions> options
    );
    std::shared_ptr<containerImageRef> makeContainerImageRef(std::shared_ptr<CommitOptions> options);
    void initConfig(std::shared_ptr<storage::Image> img,std::shared_ptr<SystemContext> sys);
    void SetAnnotation(std::string key,std::string value);
    std::string Hostname();
    std::string Domainname();
    std::string User();
    std::vector<std::string> Env();
    std::vector<std::string> Cmd();
    std::string WorkDir();
    std::vector<std::string> Entrypoint();
    std::map<std::string,std::string> Labels();
    std::vector<std::string> Shell();
    std::string StopSignal();
    std::vector<std::string> OnBuild();
    std::string Maintainer();
    std::string Architecture();
    std::string Mount(std::string label);
    void Add(std::string destination,bool extract,std::shared_ptr<AddAndCopyOptions> options,std::vector<std::string> sources);
    void Save(std::string name);
};

class BuilderInfo {
    public:
    // Type 显示容器的类型
    std::string Type;
    // FromImage 是基础镜像的名称
    std::string FromImage;
    // FromImageID 是基础镜像的ID
    std::string FromImageID;
    // FromImageDigest 是基础镜像的摘要
    std::string FromImageDigest;
    // GroupAdd 是要添加的组
    std::vector<std::string> GroupAdd;
    // Config 是容器的配置信息
    std::string Config;
    // Manifest 是容器的Manifest信息
    // std::string Manifest;
    std::shared_ptr<::Manifest> Manifest=std::make_shared<::Manifest>();
    // Container 是容器的名称
    std::string Container;
    // ContainerID 是容器的ID
    std::string ContainerID;
    // MountPoint 是容器的挂载点
    std::string MountPoint;
    // ProcessLabel 是容器的 SELinux 进程标签
    std::string ProcessLabel;
    // MountLabel 是容器的 SELinux 挂载标签
    std::string MountLabel;
    // ImageAnnotations 是镜像的注释信息
    std::map<std::string, std::string> ImageAnnotations;
    // ImageCreatedBy 表示镜像是由谁创建的
    std::string ImageCreatedBy;
    // OCIv1 是OCI v1镜像信息
    std::shared_ptr<v1::Image> OCIv1=std::make_shared<v1::Image>();
    // Docker 是Docker v2镜像信息
    // std::shared_ptr<V2Image> Docker;
    // DefaultMountsFilePath 是默认的挂载文件路径
    std::string DefaultMountsFilePath;
    // Isolation 是隔离模式
    std::string Isolation;
    // NamespaceOptions 是命名空间选项
    std::shared_ptr<::NamespaceOptions> NamespaceOptions=std::make_shared<::NamespaceOptions>();
    // Capabilities 是能力列表
    std::vector<std::string> Capabilities;
    // ConfigureNetwork 是网络配置
    std::string ConfigureNetwork;
    // CNIPluginPath 是 CNI 插件路径
    std::string CNIPluginPath;
    // CNIConfigDir 是 CNI 配置文件路径
    std::string CNIConfigDir;
    // IDMappingOptions 是用户命名空间的映射选项
    std::shared_ptr<::IDMappingOptions> IDMappingOptions=std::make_shared<::IDMappingOptions>();
    // History 是镜像的历史信息
    std::vector<::History> History;
    // Devices 是容器的设备信息
    std::shared_ptr<ContainerDevices> Devices=std::make_shared<ContainerDevices>();
    // DeviceSpecs 是设备规格信息
    std::vector<std::string> DeviceSpecs;
    // CDIConfigDir 是CDI配置文件路径
    std::string CDIConfigDir;
    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const BuilderInfo& image) {
        jv = boost::json::object{
            {"Type", image.Type},
            {"FromImage", image.FromImage},
            {"ConfigID", image.FromImageID},
            {"ManifestID", image.FromImageDigest},
            // {"GroupAdd", image.GroupAdd},
            {"Config", image.Config},
            {"Manifest", boost::json::value_from(*image.Manifest)},
            // {"Container", image.Container},
            // {"ContainerID", image.ContainerID},
            // {"MountPoint", image.MountPoint},
            // {"ProcessLabel", image.ProcessLabel},
            // {"MountLabel", image.MountLabel},
            {"ImageAnnotations", boost::json::value_from(image.ImageAnnotations)},
            {"ImageCreatedBy", image.ImageCreatedBy},
            {"OCIv1", boost::json::value_from(*image.OCIv1)},
            // {"DefaultMountsFilePath", image.DefaultMountsFilePath},
            // {"Isolation", image.Isolation},
            // {"NamespaceOptions", image.NamespaceOptions},
            // {"Capabilities", image.Capabilities},
            // {"ConfigureNetwork", image.ConfigureNetwork},
            // {"CNIPluginPath", image.CNIPluginPath},
            // {"CNIConfigDir", image.CNIConfigDir},
            // {"IDMappingOptions", image.IDMappingOptions},
            // {"History", boost::json::value_from(image.History)},
            // {"Devices", image.Devices},
            // {"DeviceSpecs", image.DeviceSpecs},
            // {"CDIConfigDir", image.CDIConfigDir}
        };
    }
};
class BuilderOptions {
    public:
    // Args 定义用户在构建时可以传递给构建器的变量
    std::map<std::string, std::string> Args;
    // FromImage 是用于容器的基础镜像，可以为空或 "scratch" 以表示不使用基础镜像
    std::string FromImage;
    // ContainerSuffix 为生成的容器名称添加的后缀
    std::string ContainerSuffix;
    // Container 是构建容器的期望名称
    std::string Container;
    // PullPolicy 决定是否拉取基础镜像
    std::shared_ptr<::PullPolicy> PullPolicy=std::make_shared<::PullPolicy>();
    // Registry 是镜像名称的前缀
    std::string Registry;
    // BlobDirectory 是用于存储层 blob 的目录
    std::string BlobDirectory;
    // GroupAdd 是需要添加的组
    std::vector<std::string> GroupAdd;
    // Logger 是 logrus 的 logger 实例
    // std::shared_ptr<::Logger> Logger=std::make_shared<::Logger>();  // 假设使用std::shared_ptr
    // Mount 信号表明容器应立即挂载
    bool Mount=false;
    // SignaturePolicyPath 指定用于签名验证的策略路径
    std::string SignaturePolicyPath;
    // ReportWriter 是用于记录镜像拉取日志的 io.Writer
    std::shared_ptr<ostream> ReportWriter=nullptr;
    // SystemContext 保存认证和授权信息
    std::shared_ptr<::SystemContext> SystemContext=std::make_shared<::SystemContext>();
    // DefaultMountsFilePath 是默认挂载文件路径
    std::string DefaultMountsFilePath;
    // Isolation 控制如何处理 "RUN" 语句
    std::shared_ptr<::Isolation> Isolation=std::make_shared<::Isolation>();
    // NamespaceOptions 控制如何设置进程的命名空间
    std::shared_ptr<::NamespaceOptions> NamespaceOptions=std::make_shared<::NamespaceOptions>();
    // ConfigureNetwork 控制是否为新网络命名空间配置网络
    std::shared_ptr<::NetworkConfigurationPolicy> ConfigureNetwork=std::make_shared<::NetworkConfigurationPolicy>();
    // CNIPluginPath 是 CNI 插件的路径
    std::string CNIPluginPath;
    // CNIConfigDir 是 CNI 配置文件的路径
    std::string CNIConfigDir;
    // NetworkInterface 是用于设置 CNI 或 netavark 网络的网络接口
    std::shared_ptr<ContainerNetwork> NetworkInterface=std::make_shared<ContainerNetwork>();
    // IDMappingOptions 是设置用户命名空间时使用的映射选项
    std::shared_ptr<::IDMappingOptions> IDMappingOptions=std::make_shared<::IDMappingOptions>();
    // Capabilities 是执行 Run() 时使用的能力列表
    std::vector<std::string> Capabilities;
    // CommonBuildOpts 是构建时的通用选项
    std::shared_ptr<CommonBuildOptions> CommonBuildOpts=std::make_shared<CommonBuildOptions>();
    // Format 是容器镜像的格式
    std::string Format;
    // Devices 是执行 Run() 时提供的设备列表
    std::shared_ptr<ContainerDevices> Devices=std::make_shared<ContainerDevices>();
    // DeviceSpecs 是执行 Run() 时提供的设备规格
    std::vector<std::string> DeviceSpecs;
    // DefaultEnv 已弃用，忽略
    std::vector<std::string> DefaultEnv;
    // MaxPullRetries 是镜像拉取失败时的最大重试次数
    int MaxPullRetries=0;
    // PullRetryDelay 是重试镜像拉取的延迟时间
    std::chrono::duration<double> PullRetryDelay=std::chrono::seconds(0);
    // OciDecryptConfig 包含解密镜像的配置
    std::shared_ptr<DecryptConfig> OciDecryptConfig=std::make_shared<DecryptConfig>();
    // ProcessLabel 是执行 Run() 时的 SELinux 进程标签
    std::string ProcessLabel;
    // MountLabel 是工作容器的 SELinux 挂载标签
    std::string MountLabel;
    // PreserveBaseImageAnns 指示是否保留基础镜像信息
    bool PreserveBaseImageAnns=false;
    // CDIConfigDir 是 CDI 配置文件的路径
    std::string CDIConfigDir;
};
class ImportOptions{
    public:
    std::string Container;
    std::string SignaturePolicyPath;
};
extern std::shared_ptr<PolicyTransportScopes> storageAllowedPolicyScopes;
bool checkRegistrySourcesAllows(std::string forWhat,std::shared_ptr<ImageReference_interface> dest);
std::shared_ptr<Builder> NewBuilder(std::shared_ptr<Store> store,std::shared_ptr<BuilderOptions> options);
#endif
