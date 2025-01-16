#if !defined(CLI_BUILD_H)
#define CLI_BUILD_H
// #include "cmd/build/build.h"
#include <string>
#include <vector>
#include <fstream>
#include "utils/cli/cobra/command.h"
#include "image/types/define/build.h"
#include "utils/cli/cli/common.h"
#include "image/types/define/types.h"
// #include ""
using std::string;
using std::vector;

/**
 * @brief BudResults 表示构建标志的结果
 * 
 * 所有平台：镜像构建是否适用所有平台
 * annotation：向镜像添加annotation
 * tag：镜像的标签
 * envs：镜像的环境变量
 * osversion：镜像支持的操作系统
 * Authfile：认证文件
 * BuildArg：向构建器提供argument和value
 * CacheFrom：用作可能的缓存源的远程存储库列表
 * CacheTo：用作可能的缓存目标的远程存储库列表
 * CacheTTL：只考虑在指定时间段内的缓存图像
 * CertDir：使用指定路径中的证书来访问注册表
 * Compress：是否压缩图像
 * File：Dockerfile的路径或URL
 * Creds：用于访问注册表的用户名和密码
 * CWOptions：机密工作负载选项
 * DisableCompression：是否禁用层压缩
 * DisableContentTrust：是否禁用内容信任
 * Format：输出格式
 * From：构建的基础镜像
 * IgnoreFile：忽略的文件或目录列表
 * Iidfile：存储镜像ID的文件
 * Jobs：并行构建的作业数
 * LogRusage：记录资源使用情况
 * RusageLogFile：记录资源使用情况的日志文件
 * LogSplitByPlatform：是否按平台分割日志
 * Label：向镜像添加标签
 * LayerLabel：向镜像的层添加标签
 * Logfile：构建日志的路径
 * Manifest：指定镜像的摘要类型
 * NoCache：是否禁用构建缓存
 * Timestamp：指定镜像的创建时间
 * OmitHistory：是否省略构建历史记录
 * OCIHooksDir：OCI钩子目录
 * Pull：是否拉取基础镜像
 * PullAlways：是否始终拉取基础镜像
 * PullNever：是否从本地不拉取基础镜像
 * Quiet：是否静默模式
 * IdentityLabel：是否添加标识标签
 * Rm：是否在构建完成后删除中间镜像
 * Stdin：是否从标准输入读取Dockerfile
 * BuildOutput：构建输出的路径
 * Target：构建的目标阶段
 * TLSVerify：是否验证注册表的TLS证书
 * OSFeatures：操作系统功能
 * OSVersion：操作系统版本
 * SignBy：用于签名的签名者
 * SignaturePolicy：签名策略
 * SkipUnusedStages：是否跳过未使用的构建阶段
 * Squash：是否压缩镜像层
 * UnsetEnvs：取消设置的环境变量
 * UnsetLabels：取消设置的标签
 * NoHostname：是否在容器中禁用主机名
 * NoHosts：是否在容器中禁用hosts文件
 * AddHost：向容器的hosts文件添加主机条目
 * Secrets：指定的密码文件
 * SSH：向容器添加SSH密钥
 */
struct BudResults{
    bool                check=false;          ///<是否生成每个文件的校验码存储文件
    bool                allplatform=false;     ///<镜像构建是否适用所有平台
    vector<string>      annotation;  ///<向镜像添加annotation
    vector<string>      tag;         ///<镜像的标签
    vector<string>      envs;        ///<镜像的环境变量
    string              osversion;           ///<镜像支持的操作系统
    string              Authfile;            ///<验证文件路径
    vector<string>      BuildArg;            ///<向构建器提供的参数
    vector<string>      CacheFrom;           ///<用作缓存源的远程仓库列表
    vector<string>      CacheTo;             ///<用作缓存目标的远程仓库列表
    string              CacheTTL;            ///<仅考虑缓存图像的时间跨度
    string              CertDir;             ///<用于访问注册表的证书的路径
    bool                Compress=false;       ///<是否压缩镜像
    vector<string>      File;                ///<Dockerfile的路径或URL
    string              Creds;               ///<用于访问注册表的凭据
    string              CWOptions;           ///<Confidential Workload选项
    bool                DisableCompression=false;  ///<是否禁用压缩
    bool                DisableContentTrust=false; ///<是否禁用内容信任
    string              Format;              ///<输出格式
    string              From;                ///<构建的基础镜像
    string              IgnoreFile;          ///<忽略的文件或目录列表
    string              Iidfile;             ///<存储镜像ID的文件
    int                 Jobs=0;              ///<并行构建的作业数
    bool                LogRusage=false;     ///<记录资源使用情况
    string              RusageLogFile;       ///<记录资源使用情况的日志文件
    bool                LogSplitByPlatform=false; ///<是否按平台分割日志
    vector<string>      Label;               ///<向镜像添加标签
    vector<string>      LayerLabel;          ///<向镜像的层添加标签
    string              Logfile;             ///<构建日志的路径
    string              Manifest;            ///<指定镜像的摘要类型
    bool                NoCache=false;       ///<是否禁用构建缓存
    int64_t             Timestamp=0;         ///<指定镜像的创建时间
    bool                OmitHistory=false;   ///<是否省略构建历史记录
    vector<string>      OCIHooksDir;         ///<OCI钩子目录
    string              Pull;                ///<是否拉取基础镜像
    bool                PullAlways=false;     ///<是否始终拉取基础镜像
    bool                PullNever=false;     ///<是否从本地不拉取基础镜像
    bool                Quiet=false;         ///<是否静默模式
    bool                IdentityLabel=false; ///<是否添加标识标签
    bool                Rm=false;            ///<是否在构建完成后删除中间镜像
    bool                Stdin;               ///<是否从标准输入读取Dockerfile
    string              BuildOutput;         ///<构建输出的路径
    string              Target;              ///<构建的目标阶段
    bool                TLSVerify=false;     ///<是否验证注册表的TLS证书
    vector<string>      OSFeatures;          ///<操作系统功能
    string              OSVersion;           ///<操作系统版本
    string              SignBy;              ///<用于签名的签名者
    string              SignaturePolicy;     ///<签名策略
    bool                SkipUnusedStages=false; ///<是否跳过未使用的构建阶段
    bool                Squash=false;         ///<是否压缩镜像层
    vector<string>      UnsetEnvs;           ///<取消设置的环境变量
    vector<string>      UnsetLabels;          ///<取消设置的标签
    bool                NoHostname=false;     ///<是否在容器中禁用主机名
    bool                NoHosts=false;        ///<是否在容器中禁用hosts文件
    vector<string>      AddHost;             ///<向容器的hosts文件添加主机条目
    vector<string>      Secrets;             ///<指定的密码文件
    vector<string>      SSH;                 ///<向容器添加SSH密钥
};
/**
 * @brief LayerResults 表示层标志的结果
 * 
 */
struct LayerResults{
    bool ForceRm=false; ///<
    bool Layers=false; ///<
};
/**
 * @brief FromAndBudResults 表示 build 和 from 命令中常见标志的结果
 * 
 * FromAndBudResults 结构定义了从 build 和 from 命令中获得的标志结果，包括：
 * - AddHost：自定义的主机到 IP 映射列表
 * - BlobCache：假设镜像的 blobs 在指定目录中可用于推送
 * - CapAdd：在运行时添加的特定功能列表
 * - CapDrop：在运行时忽略的特定功能列表
 * - CDIConfigDir：CDI 配置文件目录
 * - CgroupParent：容器的父 cgroup
 * - CPUPeriod：CPU CFS（完全公平调度器）周期限制
 * - CPUQuota：CPU CFS（完全公平调度器）配额限制
 * - CPUSetCPUs：允许执行的 CPU 列表
 * - CPUSetMems：允许执行的内存节点列表（只在 NUMA 系统上有效）
 * - CPUShares：CPU 相对权重
 * - DecryptionKeys：解密镜像所需的密钥列表
 * - Devices：要提供的附加设备列表
 * - DNSSearch：自定义 DNS 搜索域列表
 * - DNSServers：自定义 DNS 服务器列表或通过设置为 'none' 完全禁用自动创建 `/etc/resolv.conf`
 * - DNSOptions：自定义 DNS 选项列表
 * - HTTPProxy：通过 HTTP 代理环境变量传递
 * - Isolation：使用的进程隔离类型
 * - Memory：内存限制
 * - MemorySwap：内存交换空间限制
 * - Retry：重试次数
 * - RetryDelay：重试延迟
 * - SecurityOpt：安全选项列表
 * - ShmSize：共享内存大小
 * - Ulimit：用户限制列表
 * - Volumes：要挂载的卷列表
 */
struct FromAndBudResults{
    vector<string>      AddHost; ///< 自定义主机到 IP 映射列表
    string              BlobCache; ///< 假设镜像的 blobs 在指定目录中可用于推送
    vector<string>      CapAdd; ///< 在运行时添加的特定功能列表
    vector<string>      CapDrop; ///< 在运行时忽略的特定功能列表
    string              CDIConfigDir; ///< CDI 配置文件目录
    string              CgroupParent; ///< 容器的父 cgroup
    uint64_t            CPUPeriod=0; ///< CPU CFS（完全公平调度器）周期限制
    int64_t             CPUQuota=0; ///< CPU CFS（完全公平调度器）配额限制
    string              CPUSetCPUs; ///< 允许执行的 CPU 列表
    string              CPUSetMems; ///< 允许执行的内存节点列表（只在 NUMA 系统上有效）
    uint64_t            CPUShares=0; ///< CPU 相对权重
    vector<string>      DecryptionKeys; ///< 解密镜像所需的密钥列表
    vector<string>      Devices; ///< 要提供的附加设备列表
    vector<string>      DNSSearch; ///< 自定义 DNS 搜索域列表
    vector<string>      DNSServers; ///< 自定义 DNS 服务器列表或通过设置为 'none' 完全禁用自动创建 `/etc/resolv.conf`
    vector<string>      DNSOptions; ///< 自定义 DNS 选项列表
    bool                HTTPProxy=false; ///< 通过 HTTP 代理环境变量传递
    string              Isolation; ///< 使用的进程隔离类型
    string              Memory; ///< 内存限制
    string              MemorySwap; ///< 内存交换空间限制
    int                 Retry=0; ///< 重试次数
    string              RetryDelay; ///< 重试延迟
    vector<string>      SecurityOpt; ///< 安全选项列表
    string              ShmSize; ///< 共享内存大小
    vector<string>      Ulimit; ///< 用户限制列表
    vector<string>      Volumes; ///< 要挂载的卷列表
};

/**
 * @brief 用户命名空间结果
 * 
 * UserNSResults 定义了用户命名空间相关的结果，
 * 包括用户命名空间、需要添加的用户组、用户命名空间 UID 映射、
 * 用户命名空间 GID 映射、用户命名空间 UID 映射用户、
 * 用户命名空间 GID 映射用户组
 */
struct UserNSResults{
    string              UserNS; ///< 用户命名空间
    vector<string>      GroupAdd; ///< 需要添加的用户组
    vector<string>      UserNSUIDMap; ///< 用户命名空间 UID 映射
    vector<string>      UserNSGIDMap; ///< 用户命名空间 GID 映射
    string              UserNSUIDMapUser; ///< 用户命名空间 UID 映射用户
    string              UserNSGIDMapGroup; ///< 用户命名空间 GID 映射用户组
};

/**
 * @struct NameSpaceResults
 * @brief NameSpaceResults 表示名称空间结果
 * 
 * NameSpaceResults 定义了名称空间命令中合法的flag的结果，
 * 后续在功能扩展时，只需在NameSpaceResults中添加即可添加flag
 * 
 * - Cgroup: cgroup 命名空间
 * - IPC: IPC 命名空间
 * - Network: 网络 命名空间
 * - CNIConfigDir: CNI 配置目录
 * - CNIPlugInPath: CNI 插件路径
 * - PID: 进程 命名空间
 * - UTS: 主机 命名空间
 */
struct NameSpaceResults{
    string          Cgroup; ///< cgroup 命名空间
	string          IPC;   ///< IPC 命名空间
	string          Network;  ///< 网络 命名空间
	string          CNIConfigDir;  ///< CNI 配置目录
	string          CNIPlugInPath;  ///< CNI 插件路径
	string          PID;    ///< 进程 命名空间
	string          UTS;    ///< 主机 命名空间
};
/**
 * @struct BuildOptions
 * @brief BuildOptions 代表build标志的结果
 * BuildOptions 定义build命令中合法flag的声明，
 * 后续在功能扩展时，只需在BuildOptions中添加即可添加flag
 */

/**
 * @struct BuildOptions
 * @brief BuildOptions 代表build标志的结果
 * BuildOptions 定义build命令中合法flag的声明，
 * 后续在功能扩展时，只需在BuildOptions中添加即可添加flag
 * 
 * - logwriter: logwriter 日志输出流指针，默认为nullptr
 * - BudResults: BudResults 构建结果
 * - LayerResults: LayerResults 层结果
 * - FromAndBudResults: FromAndBudResults 来源和构建结果
 * - UserNSResults: UserNSResults 用户命名空间结果
 * - NameSpaceResults: NameSpaceResults 名称空间结果
 */
struct BuildOptions:public BudResults,public LayerResults,public FromAndBudResults,public UserNSResults,public NameSpaceResults{

    /// logwriter 日志输出流指针，默认为nullptr
    std::ofstream*       logwriter=nullptr;
    /// BuildOptions 构造函数，初始化BuildOptions对象
    BuildOptions():BudResults(),LayerResults(),FromAndBudResults(),UserNSResults(){};
};


void GenBuildOptions(Command* cmd, vector<string> inputArgs,BuildOptions* iopts, shared_ptr<define_BuildOptions> budopt, vector<string>& ret_containerfiles,vector<string>& removeAll);

#endif // CLI_BUILD_H
