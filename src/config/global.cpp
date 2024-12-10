#include "utils/init/init_global.h"
#include "config/config.h"
#include "config/default_linux.h"
#include "config/defaut.h"
#include "config/new.h"

// 全局变量定义
std::string _configPath;
std::string UserOverrideContainersConfig;
std::string bindirPrefix;

std::string OverrideContainersConfig;
std::string DefaultContainersConfig;
std::string DefaultSignaturePolicyPath;
rlim_t oldMaxSize;

    // _defaultGraphRoot points to the default path of the graph root.
std::string _defaultGraphRoot;
    // _defaultTransport is a prefix that we apply to an image name to check
    // docker hub first for the image.
std::string _defaultTransport ;
    // _defaultImageVolumeMode is a mode to handle built-in image volumes.
std::string _defaultImageVolumeMode;
    // defaultInitName is the default name of the init binary
std::string defaultInitName;
std::vector<std::string> DefaultMaskedPaths;
std::vector<std::string> DefaultReadOnlyPaths;
// DefaultInfraImage 是用于在 pod 中作为基础设施容器运行的默认镜像。
std::string DefaultInfraImage;
// DefaultRootlessSHMLockPath 是 rootless SHM 锁的默认路径。
std::string DefaultRootlessSHMLockPath;
// DefaultDetachKeys 是用于分离容器的默认键序列。
std::string DefaultDetachKeys;
// ErrConmonOutdated 指示找到的 conmon 版本（无论是通过配置还是 $PATH）
// 对于当前 podman 版本来说过时。
myerror ErrConmonOutdated;
// ErrInvalidArg 指示传递了无效的参数。
myerror ErrInvalidArg;
// DefaultHooksDirs 定义了默认的 hooks 目录。
std::vector<std::string> DefaultHooksDirs;
// DefaultCapabilities 是 containers.conf 文件中 default_capabilities 选项的默认值。
std::vector<std::string> DefaultCapabilities;
// 搜索这些位置以安装 CNI 插件。
std::vector<std::string> DefaultCNIPluginDirs;
std::vector<std::string> DefaultNetavarkPluginDirs;
// DefaultSubnetPools 是默认的子网池列表。
std::vector<SubnetPool> DefaultSubnetPools;
// additionalHelperBinariesDir 是额外的 helper 二进制文件目录
// 应在链接时设置，如果不同的打包程序将其放在不同的位置。
boost::optional<std::string> additionalHelperBinariesDir;
// defaultUnixComposeProviders 是默认的 Unix Compose 提供程序列表。
std::vector<std::string> defaultUnixComposeProviders;
// defaultContainerEnv 是默认的容器环境变量列表。
std::vector<std::string> defaultContainerEnv;
// std::string DefaultSignaturePolicyPath = "/etc/containers/policy.json";
// _etcDir is the sysconfdir where podman should look for system config files.
// It can be overridden at build time.
std::string _etcDir;
// InstallPrefix is the prefix where podman will be installed.
// It can be overridden at build time.
std::string _installPrefix;
// CgroupfsCgroupsManager represents cgroupfs native cgroup manager.
std::string CgroupfsCgroupsManager;
// DefaultApparmorProfile  specifies the default apparmor profile for the container.
std::string DefaultApparmorProfile;
// DefaultHostsFile is the default path to the hosts file.
std::string DefaultHostsFile;
// SystemdCgroupsManager represents systemd native cgroup manager.
std::string SystemdCgroupsManager;
// DefaultLogSizeMax is the default value for the maximum log size
// allowed for a container. Negative values mean that no limit is imposed.
int DefaultLogSizeMax;
// DefaultEventsLogSize is the default value for the maximum events log size
// before rotation.
uint64_t DefaultEventsLogSizeMax;
// DefaultPidsLimit is the default value for maximum number of processes
// allowed inside a container.,
int DefaultPidsLimit;
// DefaultPullPolicy pulls the image if it does not exist locally.
std::string DefaultPullPolicy;
// DefaultSubnet is the subnet that will be used for the default
// network.
std::string DefaultSubnet;
// DefaultRootlessSignaturePolicyPath is the location within
// XDG_CONFIG_HOME of the rootless policy.json file.
std::string DefaultRootlessSignaturePolicyPath;
// DefaultShmSize is the default upper limit on the size of tmpfs mounts.
std::string DefaultShmSize;
// DefaultUserNSSize indicates the default number of UIDs allocated for user namespace within a container.
// Deprecated: no user of this field is known.
uint64_t DefaultUserNSSize;
// OCIBufSize limits maximum LogSizeMax.
uint64_t OCIBufSize;
// SeccompOverridePath if this exists it overrides the default seccomp path.
std::string SeccompOverridePath;
// SeccompDefaultPath defines the default seccomp path.
std::string SeccompDefaultPath;
// DefaultVolumePluginTimeout is the default volume plugin timeout, in seconds
int DefaultVolumePluginTimeout;

std::shared_ptr<myerror> cachedConfigError;
std::shared_ptr<Config> cachedConfig;
std::string containersConfEnv;
std::string containersConfOverrideEnv;

/**
 * @brief 初始化config全局变量
 * 
 */
void init_config(){
    _configPath = "containers/containers.conf";
	// UserOverrideContainersConfig holds the containers config path overridden by the rootless user
	UserOverrideContainersConfig = ".config/" + _configPath;
	// Token prefix for looking for helper binary under $BINDIR
	bindirPrefix = "$BINDIR";
	// OverrideContainersConfig holds the default config path overridden by the root user
	OverrideContainersConfig = "/etc/"+_configPath;

	// DefaultContainersConfig holds the default containers config path
    DefaultContainersConfig = "/usr/share/" + _configPath;
	// DefaultSignaturePolicyPath is the default value for the
	// policy.json file.
    DefaultSignaturePolicyPath = "D:\\oci_images\\policy.json";
    // getDefaultProcessLimits 返回当前进程的nproc限制，格式为ulimits
    // 注意：nfile有时不能设置为无限制，限制硬编码为(oldMaxSize) 1048576 (2^20)，参见：http://stackoverflow.com/a/1213069/1811501
    // 在无特权容器中，这将失败，进程将使用其当前的限制
    oldMaxSize = 1048576; // 2^20
    // _defaultGraphRoot points to the default path of the graph root.
    _defaultGraphRoot = "/var/lib/containers/storage";

    // _defaultTransport is a prefix that we apply to an image name to check
    // docker hub first for the image.
    _defaultTransport = "docker://";

    // _defaultImageVolumeMode is a mode to handle built-in image volumes.
    _defaultImageVolumeMode = "anonymous";

    // defaultInitName is the default name of the init binary
    defaultInitName = "catatonit";

    // 常量定义
    DefaultMaskedPaths = {
        "/proc/acpi",
        "/proc/kcore",
        "/proc/keys",
        "/proc/latency_stats",
        "/proc/sched_debug",
        "/proc/scsi",
        "/proc/timer_list",
        "/proc/timer_stats",
        "/sys/dev/block",
        "/sys/devices/virtual/powercap",
        "/sys/firmware",
        "/sys/fs/selinux"
    };

    DefaultReadOnlyPaths = {
        "/proc/asound",
        "/proc/bus",
        "/proc/fs",
        "/proc/irq",
        "/proc/sys",
        "/proc/sysrq-trigger"
    };

    // DefaultInfraImage 是用于在 pod 中作为基础设施容器运行的默认镜像。
    DefaultInfraImage = "";
    // DefaultRootlessSHMLockPath 是 rootless SHM 锁的默认路径。
    DefaultRootlessSHMLockPath = "/libpod_rootless_lock";
    // DefaultDetachKeys 是用于分离容器的默认键序列。
    DefaultDetachKeys = "ctrl-p,ctrl-q";

    // ErrConmonOutdated 指示找到的 conmon 版本（无论是通过配置还是 $PATH）
    // 对于当前 podman 版本来说过时。
    ErrConmonOutdated.message_="outdated conmon version";
    // ErrInvalidArg 指示传递了无效的参数。
    ErrInvalidArg.message_="invalid argument";

    // DefaultHooksDirs 定义了默认的 hooks 目录。
    DefaultHooksDirs = {"/usr/share/containers/oci/hooks.d"};

    // DefaultCapabilities 是 containers.conf 文件中 default_capabilities 选项的默认值。
    DefaultCapabilities = {
        "CAP_CHOWN",
        "CAP_DAC_OVERRIDE",
        "CAP_FOWNER",
        "CAP_FSETID",
        "CAP_KILL",
        "CAP_NET_BIND_SERVICE",
        "CAP_SETFCAP",
        "CAP_SETGID",
        "CAP_SETPCAP",
        "CAP_SETUID",
        "CAP_SYS_CHROOT"
    };

    // 搜索这些位置以安装 CNI 插件。
    DefaultCNIPluginDirs = {
        "/usr/local/libexec/cni",
        "/usr/libexec/cni",
        "/usr/local/lib/cni",
        "/usr/lib/cni",
        "/opt/cni/bin"
    };

    DefaultNetavarkPluginDirs = {
        "/usr/local/libexec/netavark",
        "/usr/libexec/netavark",
        "/usr/local/lib/netavark",
        "/usr/lib/netavark"
    };

    // DefaultSubnetPools 是默认的子网池列表。
    DefaultSubnetPools = {
        parseSubnetPool("10.89.0.0/16", 24),
        parseSubnetPool("10.90.0.0/15", 24),
        parseSubnetPool("10.92.0.0/14", 24),
        parseSubnetPool("10.96.0.0/11", 24),
        parseSubnetPool("10.128.0.0/9", 24)
    };

    // additionalHelperBinariesDir 是额外的 helper 二进制文件目录
    // 应在链接时设置，如果不同的打包程序将其放在不同的位置。
    additionalHelperBinariesDir;

    // defaultUnixComposeProviders 是默认的 Unix Compose 提供程序列表。
    defaultUnixComposeProviders = {
        "docker-compose",
        "$HOME/.docker/cli-plugins/docker-compose",
        "/usr/local/lib/docker/cli-plugins/docker-compose",
        "/usr/local/libexec/docker/cli-plugins/docker-compose",
        "/usr/lib/docker/cli-plugins/docker-compose",
        "/usr/libexec/docker/cli-plugins/docker-compose",
        "podman-compose"
    };

    // defaultContainerEnv 是默认的容器环境变量列表。
    defaultContainerEnv = {"PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"};
    // std::string DefaultSignaturePolicyPath = "/etc/containers/policy.json";
    // _etcDir is the sysconfdir where podman should look for system config files.
    // It can be overridden at build time.
    _etcDir = "/etc";
    // InstallPrefix is the prefix where podman will be installed.
    // It can be overridden at build time.
    _installPrefix = "/usr";
    // CgroupfsCgroupsManager represents cgroupfs native cgroup manager.
    CgroupfsCgroupsManager = "cgroupfs";
    // DefaultApparmorProfile  specifies the default apparmor profile for the container.
    DefaultApparmorProfile = "containers-default-0.59.0-dev";
    // DefaultHostsFile is the default path to the hosts file.
    DefaultHostsFile = "/etc/hosts";
    // SystemdCgroupsManager represents systemd native cgroup manager.
    SystemdCgroupsManager = "systemd";
    // DefaultLogSizeMax is the default value for the maximum log size
    // allowed for a container. Negative values mean that no limit is imposed.
    DefaultLogSizeMax = -1;
    // DefaultEventsLogSize is the default value for the maximum events log size
    // before rotation.
    DefaultEventsLogSizeMax = 1000000;
    // DefaultPidsLimit is the default value for maximum number of processes
    // allowed inside a container.,
    DefaultPidsLimit = 2048;
    // DefaultPullPolicy pulls the image if it does not exist locally.
    DefaultPullPolicy = "missing";
    // DefaultSubnet is the subnet that will be used for the default
    // network.
    DefaultSubnet = "10.88.0.0/16";
    // DefaultRootlessSignaturePolicyPath is the location within
    // XDG_CONFIG_HOME of the rootless policy.json file.
    DefaultRootlessSignaturePolicyPath = "containers/policy.json";
    // DefaultShmSize is the default upper limit on the size of tmpfs mounts.
    DefaultShmSize = "65536k";
    // DefaultUserNSSize indicates the default number of UIDs allocated for user namespace within a container.
    // Deprecated: no user of this field is known.
    DefaultUserNSSize = 65536;
    // OCIBufSize limits maximum LogSizeMax.
    OCIBufSize = 8192;
    // SeccompOverridePath if this exists it overrides the default seccomp path.
    SeccompOverridePath = _etcDir + "/containers/seccomp.json";
    // SeccompDefaultPath defines the default seccomp path.
    SeccompDefaultPath = _installPrefix + "/share/containers/seccomp.json";
    // DefaultVolumePluginTimeout is the default volume plugin timeout, in seconds
    DefaultVolumePluginTimeout = 5;

    cachedConfigError=nullptr;
    cachedConfig=nullptr;
    containersConfEnv="CONTAINERS_CONF";
    containersConfOverrideEnv=containersConfEnv + "_OVERRIDE";
    
}