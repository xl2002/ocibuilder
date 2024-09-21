/**
 * @file Config_Json.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 声明与镜像的config文件相关的方法与类
 * @version 0.1
 * @date 2024-07-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CONFIG_CONFIG_H
#define CONFIG_CONFIG_H
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include "attributedstring/slice.h"
#include "config/pod_exit_policy.h"
#include "types/network.h"
using std::map;
using std::string;
using std::vector;



extern const std::string _configPath;
extern const std::string UserOverrideContainersConfig;
extern const std::string bindirPrefix;


struct ContainersConfig {
    // Devices to add to all containers
    std::shared_ptr<Slice> Devices=nullptr;

    // Volumes to add to all containers
    std::shared_ptr<Slice> Volumes=nullptr;

    // ApparmorProfile is the apparmor profile name which is used as the
    // default for the runtime.
    std::string ApparmorProfile;

    // Annotations to add to all containers
    std::shared_ptr<Slice> Annotations=nullptr;

    // BaseHostsFile is the path to a hosts file, the entries from this file
    // are added to the containers hosts file.
    std::string BaseHostsFile;

    // Default way to create a cgroup namespace for the container
    std::string CgroupNS;

    // Default cgroup configuration
    std::string Cgroups;

    // CgroupConf entries specifies a list of cgroup files to write to and their values.
    std::shared_ptr<Slice> CgroupConf=nullptr;

    // Capabilities to add to all containers.
    std::shared_ptr<Slice> DefaultCapabilities=nullptr;

    // Sysctls to add to all containers.
    std::shared_ptr<Slice> DefaultSysctls=nullptr;

    // DefaultUlimits specifies the default ulimits to apply to containers
    std::shared_ptr<Slice> DefaultUlimits=nullptr;

    // DefaultMountsFile is the path to the default mounts file for testing
    std::string DefaultMountsFile;

    // DNSServers set default DNS servers.
    std::shared_ptr<Slice> DNSServers=nullptr;

    // DNSOptions set default DNS options.
    std::shared_ptr<Slice> DNSOptions=nullptr;

    // DNSSearches set default DNS search domains.
    std::shared_ptr<Slice> DNSSearches=nullptr;

    // EnableKeyring tells the container engines whether to create
    // a kernel keyring for use within the container
    bool EnableKeyring = false;

    // EnableLabeling tells the container engines whether to use MAC
    // Labeling to separate containers (SELinux)
    bool EnableLabeling = false;

    // EnableLabeledUsers indicates whether to enforce confined users with
    // containers on SELinux systems.
    bool EnableLabeledUsers = false;

    // Env is the environment variable list for container process.
    std::shared_ptr<Slice> Env=nullptr;

    // EnvHost Pass all host environment variables into the container.
    bool EnvHost = false;

    // HostContainersInternalIP is used to set a specific host.containers.internal ip.
    std::string HostContainersInternalIP;

    // HTTPProxy is the proxy environment variable list to apply to container process
    bool HTTPProxy = false;

    // Init tells container runtimes whether to run init inside the
    // container that forwards signals and reaps processes.
    bool Init = false;

    // InitPath is the path for init to run if the Init bool is enabled
    std::string InitPath;

    // InterfaceName tells container runtimes how to set interface names
    std::string InterfaceName;

    // IPCNS way to create an ipc namespace for the container
    std::string IPCNS;

    // LogDriver for the container. For example: k8s-file and journald
    std::string LogDriver;

    // LogSizeMax is the maximum number of bytes after which the log file
    int64_t LogSizeMax = 0;

    // Specifies default format tag for container log messages.
    std::string LogTag;

    // Mount to add to all containers
    std::shared_ptr<Slice> Mounts=nullptr;

    // NetNS indicates how to create a network namespace for the container
    std::string NetNS;

    // NoHosts tells container engine whether to create its own /etc/hosts
    bool NoHosts = false;

    // OOMScoreAdj tunes the host's OOM preferences for containers
    std::shared_ptr<int> OOMScoreAdj;

    // PidsLimit is the number of processes each container is restricted to
    int64_t PidsLimit = 0;

    // PidNS indicates how to create a pid namespace for the container
    std::string PidNS;

    // Copy the content from the underlying image into the newly created
    // volume when the container is created.
    bool PrepareVolumeOnCreate = false;

    // Give extended privileges to all containers.
    bool Privileged = false;

    // ReadOnly causes engine to run all containers with root file system mounted read-only
    bool ReadOnly = false;

    // SeccompProfile is the seccomp.json profile path which is used as the default for the runtime.
    std::string SeccompProfile;

    // ShmSize holds the size of /dev/shm.
    std::string ShmSize;

    // TZ sets the timezone inside the container
    std::string TZ;

    // Umask is the umask inside the container.
    std::string Umask;

    // UTSNS indicates how to create a UTS namespace for the container
    std::string UTSNS;

    // UserNS indicates how to create a User namespace for the container
    std::string UserNS;

    // UserNSSize how many UIDs to allocate for automatically created UserNS
    int UserNSSize = 0;
};
struct eventsLogMaxSize;
struct SetOptions;
struct Destination;
// EngineConfig 包含用于设置引擎运行时的配置选项
struct EngineConfig {
    // CgroupCheck 表示在升级到 Fedora 31 后，配置已被重写，以更改 cgroupv2 的默认 OCI 运行时。
    bool CgroupCheck=false;

    // CGroupManager 是要使用的 CGroup 管理器。有效值为 "cgroupfs" 和 "systemd"。
    std::string CgroupManager;

    // ConmonEnvVars 是在启动 Conmon 二进制文件时传递的环境变量。
    std::shared_ptr<Slice> ConmonEnvVars=nullptr;

    // ConmonPath 是用于管理容器的 Conmon 二进制文件的路径。将使用指向有效文件的第一个路径。
    std::shared_ptr<Slice> ConmonPath=nullptr;

    // ConmonRsPath 是用于管理容器的 Conmon-rs 二进制文件的路径。将使用指向有效文件的第一个路径。
    std::shared_ptr<Slice> ConmonRsPath=nullptr;

    // CompatAPIEnforceDockerHub 强制在 Podman 的兼容性 REST API 中使用 docker.io 来完成短名称。
    bool CompatAPIEnforceDockerHub=false;

    // ComposeProviders 指定一个或多个外部提供者用于 compose 命令。第一个找到的提供者将用于执行。
    std::shared_ptr<Slice>  ComposeProviders=nullptr;

    // ComposeWarningLogs 发出日志，指示在每次调用 compose 命令时正在执行外部 compose 提供者。
    bool ComposeWarningLogs=false;

    // DBBackend 是 Podman 使用的数据库后端。
    std::string DBBackend;

    // DetachKeys 是用于分离容器的键序列。
    std::string DetachKeys;

    // EnablePortReservation 决定是否在端口转发到容器时保留主机上的端口。
    bool EnablePortReservation=false;

    // Env 是运行容器引擎（例如 Podman、Buildah）时要使用的环境变量。
    std::shared_ptr<Slice> Env=nullptr;

    // EventsLogFilePath 是事件日志存储的位置。
    std::string EventsLogFilePath;

    // EventsLogFileMaxSize 设置事件日志的最大大小。
    std::shared_ptr<eventsLogMaxSize> EventsLogFileMaxSize;

    // EventsLogger 决定事件应该记录到哪里。
    std::string EventsLogger;

    // EventsContainerCreateInspectData 创建一个更详细的容器创建事件，其中包含有关容器的详细信息的 JSON 有效负载。
    bool EventsContainerCreateInspectData=false;

    // graphRoot 内部存储 graphroot 的位置
    std::string graphRoot;

    // HelperBinariesDir 是用于搜索辅助二进制文件的目录列表。
    std::shared_ptr<Slice> HelperBinariesDir=nullptr;

    // HooksDir 是用于挂载钩子文件的目录列表。当多个目录中存在相同的文件名时，最后列出的目录中的文件具有优先权。
    std::shared_ptr<Slice>  HooksDir=nullptr;

    // ImageBuildFormat （已弃用）指示构建容器映像时的默认映像格式。应使用 ImageDefaultFormat。
    std::string ImageBuildFormat;

    // ImageDefaultTransport 是用于获取映像的默认传输方法。
    std::string ImageDefaultTransport;

    // ImageParallelCopies 指示要同时复制的映像层的最大数量。
    uint32_t ImageParallelCopies=0;

    // ImageDefaultFormat 指定在拉取、推送、构建容器映像时要使用的清单类型（oci、v2s2 或 v2s1）。
    std::string ImageDefaultFormat;

    // ImageVolumeMode 告诉容器引擎如何处理内置的映像卷。可接受的值为 "bind"、"tmpfs" 和 "ignore"。
    std::string ImageVolumeMode;

    // InfraCommand 是启动 pod 基础结构容器时运行的命令。
    std::string InfraCommand;

    // InfraImage 是 pod 基础结构容器将用于管理命名空间的映像。
    std::string InfraImage;

    // InitPath 是容器初始化二进制文件的路径。已弃用：请勿直接使用此字段，而应使用 conf.FindInitBinary()。
    std::string InitPath;

    // KubeGenerateType 设置默认情况下 podman kube generate 命令生成的 Kubernetes 种类/规范。
    std::string KubeGenerateType;

    // LockType 是要使用的锁类型。
    std::string LockType;

    // MultiImageArchive - 如果为 true，则容器引擎允许存储包含多个映像的归档（例如 docker-archive 传输）。默认情况下，Podman 创建单映像归档。
    bool MultiImageArchive=false;

    // Namespace 是要使用的引擎命名空间。命名空间用于创建作用域以分隔状态中的容器和 pod。
    std::string Namespace;

    // NetworkCmdPath 是 slirp4netns 二进制文件的路径。
    std::string NetworkCmdPath;

    // NetworkCmdOptions 是传递给 slirp4netns 二进制文件的默认选项。例如 "allow_host_loopback=true"
    std::shared_ptr<Slice> NetworkCmdOptions=nullptr;

    // NoPivotRoot 设置是否在 OCI 运行时中设置 no-pivot-root。
    bool NoPivotRoot=false;

    // NumLocks 是为容器和 pod 提供的锁数量。
    uint32_t NumLocks=0;

    // OCIRuntime 是要使用的 OCI 运行时。
    std::string OCIRuntime;

    // OCIRuntimes 是配置的 OCI 运行时集合（默认是 runc）。
    std::map<std::string, std::vector<std::string>> OCIRuntimes;

    // PlatformToOCIRuntime 请求为指定的平台映像分配特定的 OCI 运行时。
    std::map<std::string, std::string> PlatformToOCIRuntime;

    // PodExitPolicy 确定最后一个 pod 容器退出时的行为。
    std::shared_ptr<PodExitPolicy> PodExitPolicy=nullptr;

    // PullPolicy 决定在创建或运行容器之前是否拉取映像，默认是 "missing"。
    std::string PullPolicy;

    // Remote 表示应用程序是否应该在远程模式下运行。
    bool Remote=false;

    // Retry 是在拉取/推送映像失败时重试的次数。
    uint32_t Retry=0;    

    // RetryDelay 是拉取/推送映像失败时的重试间隔时间。
    std::string RetryDelay;

    // RemoteURI 已弃用，请参见 ActiveService。RemoteURI 包含用于连接到远程系统的连接信息。
    std::string RemoteURI;

    // RemoteIdentity 已弃用，见 ServiceDestinations。RemoteIdentity 是用于 RemoteURI 的密钥文件。
    std::string RemoteIdentity;

    // ActiveService 是添加的目标服务的索引（自 v2.0.3 起）。
    std::string ActiveService;

    // AddCompression 向清单列表中添加带有请求的压缩算法的现有实例。
    std::shared_ptr<Slice> AddCompression=nullptr;

    // ServiceDestinations 映射到服务名称的目标。
    std::map<std::string, Destination> ServiceDestinations;

    // SSHConfig 包含 ssh 配置文件路径（如果不是默认路径）。
    std::string SSHConfig;

    // RuntimePath 是用于启动容器的 OCI 运行时二进制文件的路径。
    std::shared_ptr<Slice> RuntimePath=nullptr;

    // RuntimeSupportsJSON 是支持 --format=json 的 OCI 运行时列表。
    std::shared_ptr<Slice> RuntimeSupportsJSON=nullptr;

    // RuntimeSupportsNoCgroups 是支持在没有 CGroup 的情况下运行容器的 OCI 运行时列表。
    std::shared_ptr<Slice> RuntimeSupportsNoCgroups=nullptr;

    // RuntimeSupportsKVM 是支持容器 KVM 分离的 OCI 运行时列表。
    std::shared_ptr<Slice> RuntimeSupportsKVM=nullptr;

    // SetOptions 包含配置选项的子集。
    std::shared_ptr<SetOptions> setOptions=nullptr;

    // SignaturePolicyPath 是用于验证映像的签名策略的路径。
    std::string SignaturePolicyPath;

    // SDNotify 告诉容器引擎允许容器使用 SD_NOTIFY 机制向主机 systemd 通知就绪状态。
    bool SDNotify=false;

    // ServiceTimeout 是在没有连接的情况下等待的秒数，然后 `podman system service` 超时并退出。
    uint32_t ServiceTimeout=0;

    // StaticDir 是用于存储容器文件的持久目录的路径。
    std::string StaticDir;

    // StopTimeout 是在发送 kill 信号之前等待容器退出的秒数。
    uint32_t StopTimeout=0;

    // ExitCommandDelay 是等待退出命令发送到服务器上的 API 进程的秒数。
    uint32_t ExitCommandDelay=0;

    // ImageCopyTmpDir 是用于存储临时容器映像内容的默认位置。
    std::string ImageCopyTmpDir;

    // TmpDir 是用于存储每次引导的容器文件的临时目录的路径。必须存储在 tmpfs 中。
    std::string TmpDir;

    // VolumePath 是将在其下创建命名卷的默认位置。
    std::string VolumePath;

    //VolumePluginTimeout 为必须联系卷插件的操作设置默认超时（以秒为单位）。
    uint32_t VolumePluginTimeout=0;

    //VolumePlugins 是一组插件，可以用作 Podman 命名卷的后端。
    std::map<std::string, std::string> VolumePlugins;

    //ChownCopiedFiles 告诉容器引擎是否将复制到容器中的文件 chown 到容器的主 uid/gid。
    bool ChownCopiedFiles=false;

    //CompressionFormat是用于压缩图像层的压缩格式。
    std::string CompressionFormat;

    //CompressionLevel 是用于压缩图像层的压缩级别。
    std::shared_ptr<int> CompressionLevel=nullptr;

    //PodmanshTimeout 是等待 podmansh 登录的秒数。
    uint32_t PodmanshTimeout=0;
};
struct SetOptions{
    // StorageConfigRunRootSet indicates if the RunRoot has been explicitly set
	// by the config or by the user. It's required to guarantee backwards
	// compatibility with older versions of libpod for which we must query the
	// database configuration. Not included in the on-disk config.
	bool StorageConfigRunRootSet=false;

	// StorageConfigGraphRootSet indicates if the RunRoot has been explicitly
	// set by the config or by the user. It's required to guarantee backwards
	// compatibility with older versions of libpod for which we must query the
	// database configuration. Not included in the on-disk config.
	bool StorageConfigGraphRootSet=false;

	// StorageConfigGraphDriverNameSet indicates if the GraphDriverName has been
	// explicitly set by the config or by the user. It's required to guarantee
	// backwards compatibility with older versions of libpod for which we must
	// query the database configuration. Not included in the on-disk config.
	bool StorageConfigGraphDriverNameSet=false;
};
struct Destination{
    //URI，必填。示例： ssh://root@example.com:22/run/podman/podman.sock
    std::string URI;
    //带有 ssh 密钥的身份文件，可选
	std::string Identity;

    //isMachine 描述远程目标是否是机器。
	bool IsMachine=false;
};
struct eventsLogMaxSize{
    uint64_t value=0;
    eventsLogMaxSize()=default;
    eventsLogMaxSize(uint64_t v):value(v){};
};

struct SubnetPool{
    std::shared_ptr<IPNet> Base=nullptr;
    int Size=0;
    SubnetPool(std::shared_ptr<IPNet> base, int size)
    : Base(base), Size(size) {}
};
// NetworkConfig 表示“network” TOML 配置表
struct NetworkConfig {
    // NetworkBackend 决定应使用 Podman 的哪个网络后端
    std::string NetworkBackend;

    // CNIPluginDirs 是存储 CNI 插件二进制文件的目录
    std::shared_ptr<Slice> CNIPluginDirs=nullptr;

    // NetavarkPluginDirs 是包含 netavark 插件的目录列表
    std::shared_ptr<Slice> NetavarkPluginDirs=nullptr;

    // FirewallDriver 是要使用的防火墙驱动程序
    std::string FirewallDriver;

    // DefaultNetwork 是要将 pod 附加到的默认网络的网络名称
    std::string DefaultNetwork;

    // DefaultSubnet 是要用于默认网络的子网
    std::string DefaultSubnet;

    // DefaultSubnetPools 是一个子网和大小的列表，用于自动分配子网
    std::vector<SubnetPool> DefaultSubnetPools;

    // DefaultRootlessNetworkCmd 用于设置默认的无根网络程序
    std::string DefaultRootlessNetworkCmd;

    // NetworkConfigDir 是存储网络配置文件的目录
    std::string NetworkConfigDir;

    // DNSBindPort 是 DNS 转发守护进程使用的端口
    uint16_t DNSBindPort = 0;

    // PastaOptions 包含运行 pasta 时应使用的默认选项列表
    std::shared_ptr<Slice> PastaOptions=nullptr;
};

struct SecretConfig{
    // Base is a bigger subnet which will be used to allocate a subnet with
	// the given size.
    std::string Driver;
    // Size is the CIDR for the new subnet. It must be equal or small
	// than the CIDR from the base subnet.
    std::map<std::string, std::string> Opts;
};

struct ConfigMapConfig{
	// Driver specifies the configmap driver to use.
	// Current valid value:
	//  * file
	//  * pass
	std::string Driver;
	// Opts contains driver specific options
	std::map<std::string, std::string> Opts;
};
// MachineConfig 代表 "machine" TOML 配置表
struct MachineConfig {
    // 机器创建时的 CPU 数量。
    uint64_t CPUs=0; // toml:"cpus,omitempty,omitzero"
    
    // 初始化 podman-machine 虚拟机时创建的磁盘大小（单位：GB）。
    uint64_t DiskSize=0; // toml:"disk_size,omitempty,omitzero"
    
    // 初始化 podman-machine 虚拟机时使用的镜像。
    std::string Image; // toml:"image,omitempty"
    
    // 机器创建时的内存大小（单位：MB）。
    uint64_t Memory=0; // toml:"memory,omitempty,omitzero"
    
    // 初始化 podman-machine 虚拟机时使用的 rootless podman 用户。
    std::string User; // toml:"user,omitempty"
    
    // 默认挂载到虚拟机中的主机目录。
    std::shared_ptr<Slice> Volumes=nullptr; // toml:"volumes,omitempty"
    
    // 用于运行 podman-machine 虚拟机的虚拟化提供者。
    std::string Provider; // toml:"provider,omitempty"
    
    // 在 Apple Silicon 上启用 Rosetta 的标志。
    bool Rosetta=false; // toml:"rosetta,omitempty"
};

struct FarmConfig{
	// Default is the default farm to be used when farming out builds
	std::string Default;
	// List is a map of farms created where key=farm-name and value=list of connections
	std::map<std::string, std::string> List;
};
/**
 * @brief 保存config文件需要的具体信息
 * 
 */
class Config{
    public:
    std::shared_ptr<ContainersConfig>    Containers=nullptr;
    std::shared_ptr<EngineConfig>        Engine=nullptr;
    std::shared_ptr<MachineConfig>       Machine=nullptr;
    std::shared_ptr<NetworkConfig>       Network=nullptr;
    std::shared_ptr<SecretConfig>        Secrets=nullptr;
    std::shared_ptr<ConfigMapConfig>     ConfigMaps=nullptr;
    std::shared_ptr<FarmConfig>          Farms=nullptr;
    std::vector<std::string>             loadedModules;
    // Config()=default;
    void CheckCgroupsAndAdjustConfig();
    std::vector<std::string> Capabilities(const std::string& user,
                                    const std::vector<std::string>& addCapabilities,
                                    const std::vector<std::string>& dropCapabilities);
    void addCAPPrefix();
    void Validate();
    void setupEnv();
    std::vector<std::string> Volumes();
    std::vector<std::string> GetDefaultEnv();
    std::vector<std::string> GetDefaultEnvEx(bool envHost,bool httpProxy);
};

struct DecryptConfig{
    map<string,vector<vector<uint8_t>>>Parameters;
};
struct EncryptConfig{
    std::map<std::string,std::vector<std::vector<uint8_t>>> Parameters;
    std::shared_ptr<DecryptConfig> DecryptConfig=nullptr;
};
struct CryptoConfig{
    std::shared_ptr<EncryptConfig> EncryptConfig=nullptr;
    std::shared_ptr<DecryptConfig> DecryptConfig=nullptr;
};
#endif