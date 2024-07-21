/**
 * @file define.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#if !defined(DEFINE_BUILD_H)
#define DEFINE_BUILD_H
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <functional>
#include "define/pull.h"
#include "docker/reference.h"
#include "config/config.h"
#include "define/namespace.h"
#include "define/types.h"
#include <memory>
// #include "pkg/parse/parse.h"
#include <chrono>
using std::string;
using std::map;
using std::vector;
using std::shared_ptr;
using std::function;

enum isolation{
    IsolationDefault,
    IsolationOCI,
    IsolationChroot,
    IsolationOCIRootless
};
class Isolation{
    public:
    isolation value;
	Isolation()=default;
    Isolation(isolation v):value(v){};
    string String();
};

enum compression{
    Uncompressed,
    Bzip2,
    Gzip,
    Xz,
    Zstd
};
class Compression{
	public:
    compression value;
	Compression()=default;
    Compression(compression v):value(v){};
    string String();
};


struct AdditionalBuildContext {
	// 值是外部 tar 存档的 URL。
	bool IsURL;
	// 值是可能已提取或尚未提取的图像的名称。
	bool IsImage;
	// 值包含 URL、图像名称或绝对文件系统路径。
	string Value;
	// 下载和导出的构建上下文的绝对文件系统路径
	//来自外部 tar 存档。仅当遵循以下内容时才会填充此内容
	//buildcontext是从IsURL创建的，并且之前在任何地方都下载过
	//运行步骤。
	string DownloadedCache ;
};
// struct ConfidentialWorkloadOptions {
// 	bool Convert=false              ;
// 	string AttestationURL     ;
// 	int CPUs=0                     ;
// 	int Memory=0                   ;
// 	string TempDir                  ; // 用于磁盘映像的临时明文副本
// 	string TeeType                  ;
// 	bool IgnoreAttestationErrors=false    ;
// 	string WorkloadID               ;
// 	string DiskEncryptionPassphrase ;
// 	string Slop                     ;
// 	string FirmwareLibrary          ;
// };
enum networkConfigurationPolicy{
    NetworkDefault,
    NetworkDisabled,
    NetworkEnabled
};
class NetworkConfigurationPolicy{
    public:
    networkConfigurationPolicy value;
	NetworkConfigurationPolicy()=default;
    NetworkConfigurationPolicy(networkConfigurationPolicy v):value(v){};
    string String();
};
struct platforms{
    std::string OS;
    std::string Arch;
    std::string Variant; 
};

///CommonBuildOptions 是可以通过 buildah from 和 build 的标志定义的资源
struct CommonBuildOptions  {
	// AddHost 是要添加到构建容器的 /etc/hosts 的主机名列表。
	vector<string> AddHost;
	//OmitHistory 告诉构建器忽略构建层的历史记录
	//准备 image-spec 时的基础，将其设置为 true 将确保没有历史记录
	//添加到图像规范中。 （默认为假）
	bool OmitHistory;
	//Cgroup Parent 是 cgroup 的路径，将在该路径下创建容器的 cgroup。
	string CgroupParent;
	// CPUPeriod 限制CPU CFS（完全公平调度器）周期
	uint64_t CPUPeriod;
	// CPUQuota限制CPU CFS（Completely Fair Scheduler）配额
	int64_t CPUQuota ;
	// CPUShares（相对权重
	uint64_t CPUShares;
	// CPUSet 允许执行的 CPU (0-3, 0,1)
	string CPUSetCPUs;
	// CPUSetMems 允许执行的内存节点 (MEM) (0-3, 0,1)。仅对 NUMA 系统有效。
	string CPUSetMems;
	//HTTPProxy 确定来自构建主机的 *_proxy 环境变量是否传递到容器中。
	bool HTTPProxy;
	//IdentityLabel 如果设置，可确保默认的 `io.buildah.version` 标签不会应用于构建映像。
	int8_t IdentityLabel;
	//内存是运行容器可以使用多少内存的上限（以字节为单位）。
	int64_t Memory;
	//DNSSearch 是要添加到构建容器的 /etc/resolv.conf 的 DNS 搜索域列表
	vector<string> DNSSearch;
	//DNS Servers 是要添加到构建容器 /etc/resolv.conf 的 DNS 服务器列表
	vector<string> DNSServers;
	//DNSOptions 是 DNS 列表
	vector<string> DNSOptions;
	//LabelOpts 是 SELinux 上下文的字段切片，以“field:pair”格式或“disable”格式给出。
	//可识别的字段名称为“角色”、“类型”和“级别”。
	vector<string> LabelOpts;
	//MemorySwap 限制内存和交换的数量。
	int64_t MemorySwap;
	//NoHostname 告诉构建器在运行时不要创建 /etc/hostname 内容
	//容器。
	bool NoHostname ;
	//NoHosts 告诉构建器在运行时不要创建 /etc/hosts 内容
	//容器。
	bool NoHosts;
	//NoNewPrivileges 删除容器获取权限的能力
	bool NoNewPrivileges;
	// OmitTimestamp 强制纪元 0 作为创建的时间戳以允许
	//确定性、内容可寻址的构建。
	bool OmitTimestamp;
	// SeccompProfilePath 是 seccomp 配置文件的路径名。
	string SeccompProfilePath;
	// ApparmorProfile 是apparmor 配置文件的名称。
	string ApparmorProfile;
	//ShmSize 是在容器的 /dev/shm 目录上安装 shmfs 时使用的“大小”值。
	string ShmSize;
	//ulimit 指定资源限制选项，格式为 type:softlimit[:hardlimit]。
	//这些类型被识别：
	//“core”：最大核心转储大小（ulimit -c）
	//“cpu”：最大CPU时间（ulimit -t）
	//“data”：进程数据段的最大大小（ulimit -d）
	//"fsize": 新文件的最大大小 (ulimit -f)
	//"locks": 文件锁的最大数量 (ulimit -x)
	//"memlock": 锁定内存的最大量 (ulimit -l)
    //"msgqueue": 消息队列中的最大数据量 (ulimit -q)
	//"nice": 友好度调整 (nice -n, ulimit -e)
	//"nofile": 最大打开文件数 (ulimit -n)
	//"nproc": 最大进程数 (ulimit -u)
	//“rss”：进程的最大大小 (ulimit -m)
	//“rtprio”：最大实时调度优先级（ulimit -r）
	//“rttime”：阻塞系统调用之间的最大实时执行量
	//"sigpending": 待处理信号的最大数量 (ulimit -i)
    //“stack”：最大堆栈大小（ulimit -s）
	vector<string> Ulimit;
	// 要绑定安装到容器中的卷
	vector<string> Volumes;
	// Secrets 是在构建中使用的可用秘密。  中的每一项
	//切片采用“id=foo,src=bar”的形式，其中“id”和“src”都是
	//按此顺序是必需的，“bar”是文件名。
	vector<string> Secrets;
	// SSHSources 是在构建中转发的可用 ssh 代理连接
	vector<string> SSHSources;
	//OCIHooksDir 是构建容器的 OCI 挂钩的位置
	vector<string> OCIHooksDir;
};

// struct Time  {
//     //wall 和 ext 对 wall time 秒、wall time 纳秒进行编码，
// 	//和可选的单调时钟读数（以纳秒为单位）。
// 	//
// 	//从高位到低位位置，wall编码一个1位标志（hasMonotonic），
// 	//33 位秒字段和 30 位墙壁时间纳秒字段。
// 	//纳秒字段的范围是 [0, 999999999]。
// 	//如果 hasMonotonic 位为 0，则 33 位字段必须为零
// 	//自 1 年 1 月 1 日起完整签名的 64 位墙秒存储在 ext.
//     //如果 hasMonotonic 位为 1，则 33 位字段保存 33 位
// 	//自 1885 年 1 月 1 日以来的无符号墙秒，并且 ext 持有
// 	//带符号的 64 位单调时钟读数，自进程启动以来的纳秒数。
// 	uint64_t wall;
// 	int64_t ext;

// 	// loc specifies the Location that should be used to
// 	// determine the minute, hour, month, day, and year
// 	// that correspond to this Time.
// 	// The nil location means UTC.
// 	// All UTC times are represented with loc==nil, never loc==&utcLoc.
// 	// loc *Location
// };
class Duration{

};
class Weighted{

};
// class SBOMScanOptions{

// };
class define_BuildOptions{
	public:
	// ContainerSuffix 为容器添加后缀的名称
	string ContainerSuffix;
	// ContextDirectory 是 COPY 和 ADD 命令的默认源位置。
	string ContextDirectory;
	// PullPolicy 控制我们是否拉取镜像。  它应该是 PullIfMissing、PullAlways、PullIfNewer 或 PullNever 之一。
	PullPolicy PullPolicy;
    //注册表是一个附加在图像名称前面的值（如果它是）
	//需要拉取，并且图像名称单独无法解析为
	//对源图像的引用。  没有隐式添加分隔符。
	string Registry;
	//IgnoreUnrecognizedInstructions 告诉我们只记录我们的指令
	//不认识，并尝试继续前进。
	bool IgnoreUnrecognizedInstructions;
	// 将添加图像的Manifest名称。
	string Manifest;
	// Quiet告诉我们是否在执行步骤时宣布这些步骤。
	bool Quiet;
	// Isolation控制 Run() 如何运行事物。
	shared_ptr<Isolation> Isolation=nullptr;
	//Runtime 是 RUN 指令运行时的命令名称
	//隔离是 IsolationDefault 或 IsolationOCI。  它应该
	//接受与 runc 相同的参数和标志。
	string Runtime;
	// RuntimeArgs 添加运行时的全局参数。
	vector<string> RuntimeArgs;
	//TransientMounts 是一个未解析的挂载列表，将提供给
	//运行指令。
	vector<string> TransientMounts;
	//CacheFrom 指定任何远程存储库，可以将其视为
	//潜在的缓存源。
	vector<named> CacheFrom;
	//CacheTo 指定任何可以被视为的远程存储库
	//潜在的缓存目的地。
	vector<named> CacheTo;
	//CacheTTL指定持续时间，如果使用`--cache-ttl`指定则
	//在此持续时间内缓存中间图像将被视为
	//在此持续时间之外的有效缓存源和图像将被忽略。
	std::chrono::duration<int> CacheTTL;
	//压缩指定应用的压缩类型
	//图层斑点。  默认不使用压缩，但是
	//建议使用 archive.Gzip。
	Compression Compression;
	//可以插入到 Dockerfile 中的参数
	map<string,string>Args;
	//外部附加构建上下文的映射
	map<string,shared_ptr<AdditionalBuildContext>> AdditionalBuildContexts;
	// 要写入的图像的名称。
	string Output ;
	//BuildOutput 指定是否为后续构建选择任何自定义构建输出。
	//它允许最终用户将最近构建的 rootfs 导出到目录或 tar 中。
	//有关格式的详细信息，请参阅“buildah build --output”的文档。
	string BuildOutput ;
	//ConfidentialWorkload 控制我们是否生成一个
	//使用 krun 作为虚拟机而不是传统的虚拟机来运行的映像
	//进程类型容器。
	ConfidentialWorkloadOptions ConfidentialWorkload ;
	//添加到我们编写的图像中的附加标签（如果我们知道）
	//添加它们的方法。
	vector<string> AdditionalTags;
	// Logfile 指定日志输出是否重定向到外部文件
	//而不是标准输出，标准错误。
	string LogFile;
	// LogByPlatform 告诉 imagebuildah 将日志拆分到不同的日志文件
	//对于每个平台（如果选择了记录到外部文件）。
	bool LogSplitByPlatform ;
	// Log 是一个将打印进度消息的回调。  如果没有值
	//提供后，消息将被发送到 Err（或 os.Stderr，如果 Err
	//为零）默认情况下。
	std::function<void(string format,vector<string>args)> Log;
	// In 连接到标准输入以获取 RUN 指令。
	std::istream* In=nullptr;
	// Out 是发送非错误日志消息的地方。
	std::ostream* Out=nullptr ;
	// Err 是应该发送错误日志消息的地方。
	std::ostream* Err=nullptr ;
	// SignaturePolicyPath 指定签名的覆盖位置
	//用于验证新图像的策略
	//正在写入。  除非在特定情况下，否则不应有任何值
	//指定，表示共享的、系统范围的默认策略
	//应该使用。
	string SignaturePolicyPath ;
	//SkipUnusedStages 允许用户在多阶段构建中跳过阶段
	//对目标阶段没有任何贡献。预期违约
	//值为真。
	OptionalBool SkipUnusedStages;
	//ReportWriter 是一个 io.Writer，用于报告
	//（可能的）拉动源图像和
	//写入新图像。
	std::ostream* ReportWriter=nullptr;
	//OutputFormat 是输出图像清单的格式
	//配置数据。
	//接受的值为 buildah.OCIv1ImageManifest 和 buildah.Dockerv2ImageManifest。
	string OutputFormat ;
	// SystemContext 保存用于身份验证的参数。
	shared_ptr< SystemContext> SystemContext;
	//NamespaceOptions 控制我们如何设置命名空间进程
	//处理 RUN 指令时可能需要。
	std::vector<NamespaceOption> Namespaceoptions;
	//ConfigureNetwork 控制是否使用网络接口
	//路由是为新的网络命名空间配置的（即，当没有
	//加入另一个名称空间而不仅仅是使用主机的名称空间
	//命名空间），有效地决定进程是否有
	//可用网络。
	NetworkConfigurationPolicy ConfigureNetwork ;
	//CNIPluginPath 是 CNI 插件助手的位置（如果它们应该是）
	//从默认位置以外的位置运行。
	string CNIPluginPath ;
	//CNIConfigDir 是 CNI 配置文件的位置，如果文件位于
	//不应使用默认配置目录。
	string CNIConfigDir ;
	//NetworkInterface 是用于设置 CNI 或 netavark 网络的 libnetwork 网络接口。
	// NetworkInterface nettypes.ContainerNetwork `json:"-"`;

	//如果我们要设置自己的用户命名空间，则使用 ID 映射选项
	//处理 RUN 指令时。
	shared_ptr<IDMappingOptions> IDMappingoptions=nullptr;
	//AddCapability 是添加到默认集的功能列表
	//处理 RUN 指令。
	vector<string> AddCapabilities  ;
	//DropCapability 是要从默认集中删除的功能列表
	//处理 RUN 指令时。如果某项功能同时出现在两个列表中，则它
	//将被丢弃。
	vector<string> DropCapabilities  ;
	// CommonBuildOpts 是*必需的*。
	shared_ptr<CommonBuildOptions> CommonBuildOpts =nullptr;
	// CPPFlags 是传递给 C 预处理器 (cpp) 的附加参数。
	vector<string> CPPFlags ;
	//DefaultMountsFilePath 是保存要挂载的 RUN 挂载的文件路径
	//“主机路径:容器路径”格式的指令
	string DefaultMountsFilePath ;
	// IIDFile 告诉构建器将图像 ID 写入指定文件
	string IIDFile ;
	//Squash 告诉构建器生成带有单层而不是带有层的图像
	//可能不止一层，通过在处理后仅提交一个新层
	//最终指令。
	bool Squash  ;
	//在提交的图像中设置的标签。
	vector<string> Labels ;
	// 中间图像的 LayerLabels 元数据
	vector<string> LayerLabels ;
	// 以 OCI 格式在提交的图像中设置的注释。
	vector<string> Annotations  ;
	// OnBuild 命令由使用我们将提交作为基础映像的映像的构建运行。
	vector<string> OnBuild  ;
	// Layers 告诉构建器为 Dockerfile 中的每个步骤提交一个镜像。
	bool Layers ;
	//NoCache 告诉构建器从头开始构建图像，而不检查缓存。
	//它为构建创建一组新的缓存图像。
	bool NoCache  ;
	//RemoveIntermediateCtrs 告诉构建器是否删除使用的中间容器
	//在构建过程中。默认为 true。
	bool RemoveIntermediateCtrs ;
	//ForceRmIntermediateCtrs 告诉构建器删除所有中间容器，即使
	//构建失败。
	bool ForceRmIntermediateCtrs;
	//BlobDirectory 是我们将用于缓存层 blob 的目录。
	string BlobDirectory ;
	//以 Dockerfile 中的目标 FROM 为目标进行构建。
	string Target ;
	//设备是未解析的设备，提供给 RUN 指令。
	vector<string> Devices;
	//SignBy 是用于签名图像的 GPG 密钥的指纹。
	string SignBy ;
	//Architecture 指定要构建的镜像的目标架构。
	string Architecture ;
	//Timestamp 将创建的时间戳设置为指定时间，允许
	//用于确定性、内容可寻址的构建。
	std::shared_ptr<std::chrono::system_clock::time_point> Timestamp=nullptr;
	//OS 指定要构建的镜像的操作系统。
	string OS ;
	//MaxPullPushRetries 是我们拉或推任何一个的最大尝试次数
	//如果第一次尝试失败，则将图像从外部注册表发送到外部注册表。
	int MaxPullPushRetries;
	//PullPushRetryDelay 是重试拉或推尝试之前等待的时间。
	Duration PullPushRetryDelay;
	//OciDecryptConfig 包含可用于解密图像的配置（如果是）
	//如果非零则加密。如果为零，则不会尝试解密图像。
	shared_ptr<DecryptConfig> OciDecryptConfig=nullptr;
	//Jobs 是并行运行的阶段数。  如果未指定，则默认为 1。
	//如果提供了 JobSemaphore，则忽略。
	shared_ptr<int> Jobs=nullptr;
	//JobSemaphore，当您希望与更多构建共享作业时。
	shared_ptr<Weighted>JobSemaphore=nullptr;
	//LogRusage 记录每个步骤的资源使用情况。
	bool LogRusage ;
	//Rusage 日志将被保存到的文件，而不是 stdout。
	string RusageLogFile ;
	//Excludes 是要使用的排除列表，而不是 .dockerignore 文件。
	vector<string> Excludes ;
	//IgnoreFile 是 .containerignore 文件的名称
	string IgnoreFile ;
	//From 是用于替换第一个中指定的值的图像名称
	//Containerfile 中的 FROM 指令。
	string From ;
	//GroupAdd 是处理 RUN 时添加到主进程的组列表
	//指示。神奇的“keep-groups”值表明该进程应该
	//允许继承当前的补充组集。
	vector<string> GroupAdd ;
	//Platforms 是我们想要的已解析 OS/Arch/Variant 三元组的列表
	//为其构建图像。  如果该切片中有项目，则操作系统和
	//上面的架构字段被忽略。
	vector<platforms> PlatformsList;
	//AllPlatforms 告诉构建器设置目标平台列表
	//匹配所有构建基础的平台集
	//图像可用。  如果设置此字段，则平台将被忽略。
	bool AllPlatforms ;
	//UnsetEnvs 是不添加到最终图像的环境列表。
	vector<string> UnsetEnvs ;
	//UnsetLabels 是不从基础图像添加到最终图像的标签列表。
	vector<string> UnsetLabels ;
	//envs 是要在最终映像中设置的环境变量列表。
	vector<string> Envs ;
	//OSFeatures 指定映像所需的操作系统功能。
	//通常仅当操作系统为“windows”时才设置。
	vector<string> OSFeatures;
	//OSVersion 指定映像的确切操作系统版本
	//需要。  它通常仅在操作系统为“windows”时设置。  任何
	//基础图像中设置的值将被保留，所以这不会
	//经常需要设置。
	string OSVersion;
	//SBOMScanOptions 封装了控制是否我们扫描的选项
	//在我们要提交的 rootfs 上运行扫描器，以及如何运行。
	vector<SBOMScanOptions> SBOMScanOptions;
	//CDIConfigDir 是 CDI 配置文件的位置，如果文件位于
	//不应使用默认配置位置。
	string CDIConfigDir;
	define_BuildOptions()=default;
};

#endif // DEFINE_DEFINE_H
