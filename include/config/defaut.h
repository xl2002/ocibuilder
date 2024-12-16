#if !defined(CONFIG_DEFAUT_H)
#define CONFIG_DEFAUT_H
#include <string>
#include <vector>
#include <memory>
#include <tuple>

#include "config/config.h"
#include "utils/common/error.h"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
// 全局变量外部声明
// _defaultGraphRoot points to the default path of the graph root.
extern std::string _defaultGraphRoot;
// _defaultTransport is a prefix that we apply to an image name to check
// docker hub first for the image.
extern std::string _defaultTransport ;
// _defaultImageVolumeMode is a mode to handle built-in image volumes.
extern std::string _defaultImageVolumeMode;
// defaultInitName is the default name of the init binary
extern std::string defaultInitName;
extern std::vector<std::string> DefaultMaskedPaths;
extern std::vector<std::string> DefaultReadOnlyPaths;
// DefaultInfraImage 是用于在 pod 中作为基础设施容器运行的默认镜像。
extern std::string DefaultInfraImage;
// DefaultRootlessSHMLockPath 是 rootless SHM 锁的默认路径。
extern std::string DefaultRootlessSHMLockPath;
// DefaultDetachKeys 是用于分离容器的默认键序列。
extern std::string DefaultDetachKeys;
// ErrConmonOutdated 指示找到的 conmon 版本（无论是通过配置还是 $PATH）
// 对于当前 podman 版本来说过时。
extern myerror ErrConmonOutdated;
// ErrInvalidArg 指示传递了无效的参数。
extern myerror ErrInvalidArg;
// DefaultHooksDirs 定义了默认的 hooks 目录。
extern std::vector<std::string> DefaultHooksDirs;
// DefaultCapabilities 是 containers.conf 文件中 default_capabilities 选项的默认值。
extern std::vector<std::string> DefaultCapabilities;
// 搜索这些位置以安装 CNI 插件。
extern std::vector<std::string> DefaultCNIPluginDirs;
extern std::vector<std::string> DefaultNetavarkPluginDirs;
// DefaultSubnetPools 是默认的子网池列表。
extern std::vector<SubnetPool> DefaultSubnetPools;
// additionalHelperBinariesDir 是额外的 helper 二进制文件目录
// 应在链接时设置，如果不同的打包程序将其放在不同的位置。
extern boost::optional<std::string> additionalHelperBinariesDir;
// defaultUnixComposeProviders 是默认的 Unix Compose 提供程序列表。
extern std::vector<std::string> defaultUnixComposeProviders;
// defaultContainerEnv 是默认的容器环境变量列表。
extern std::vector<std::string> defaultContainerEnv;
// std::string DefaultSignaturePolicyPath = "/etc/containers/policy.json";
// _etcDir is the sysconfdir where podman should look for system config files.
// It can be overridden at build time.
extern std::string _etcDir;
// InstallPrefix is the prefix where podman will be installed.
// It can be overridden at build time.
extern std::string _installPrefix;
// CgroupfsCgroupsManager represents cgroupfs native cgroup manager.
extern std::string CgroupfsCgroupsManager;
// DefaultApparmorProfile  specifies the default apparmor profile for the container.
extern std::string DefaultApparmorProfile;
// DefaultHostsFile is the default path to the hosts file.
extern std::string DefaultHostsFile;
// SystemdCgroupsManager represents systemd native cgroup manager.
extern std::string SystemdCgroupsManager;
// DefaultLogSizeMax is the default value for the maximum log size
// allowed for a container. Negative values mean that no limit is imposed.
extern int DefaultLogSizeMax;
// DefaultEventsLogSize is the default value for the maximum events log size
// before rotation.
extern uint64_t DefaultEventsLogSizeMax;
// DefaultPidsLimit is the default value for maximum number of processes
// allowed inside a container.,
extern int DefaultPidsLimit;
// DefaultPullPolicy pulls the image if it does not exist locally.
extern std::string DefaultPullPolicy;
// DefaultSubnet is the subnet that will be used for the default
// network.
extern std::string DefaultSubnet;
// DefaultRootlessSignaturePolicyPath is the location within
// XDG_CONFIG_HOME of the rootless policy.json file.
extern std::string DefaultRootlessSignaturePolicyPath;
// DefaultShmSize is the default upper limit on the size of tmpfs mounts.
extern std::string DefaultShmSize;
// DefaultUserNSSize indicates the default number of UIDs allocated for user namespace within a container.
// Deprecated: no user of this field is known.
extern uint64_t DefaultUserNSSize;
// OCIBufSize limits maximum LogSizeMax.
extern uint64_t OCIBufSize;
// SeccompOverridePath if this exists it overrides the default seccomp path.
extern std::string SeccompOverridePath;
// SeccompDefaultPath defines the default seccomp path.
extern std::string SeccompDefaultPath;
// DefaultVolumePluginTimeout is the default volume plugin timeout, in seconds
extern int DefaultVolumePluginTimeout;
extern std::string TypeLayers;
std::shared_ptr<Config> defaultConfig();
std::tuple<std::shared_ptr<EngineConfig>,boost::optional<myerror>> defaultEngineConfig();
std::tuple<std::string,boost::optional<myerror>> defaultTmpDir();
std::vector<std::string> getDefaultComposeProviders();
// std::vector<std::string> getDefaultMachineVolumes();
std::shared_ptr<SecretConfig> defaultSecretConfig();
std::shared_ptr<MachineConfig> defaultMachineConfig();
std::shared_ptr<FarmConfig> defaultFarmConfig();
std::string getDefaultTmpDir();
std::string defaultCgroupManager();
std::string defaultEventsLogger();
std::string defaultLogDriver();
std::string getDefaultSSHConfig();
SubnetPool parseSubnetPool(const std::string& subnet, int size);
std::string getDefaultMachineUser();







#endif // CONF
