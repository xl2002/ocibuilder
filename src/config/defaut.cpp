#include "config/defaut.h"
#include "config/config_local.h"
#include "storage/types/options.h"
#include "config/default_linux.h"
#include "filesys/system/unshare_linux.h"
#include "filesys/system/cgroups_linux.h"
#include <cstdlib>
#include <thread>
#include <boost/compute/detail/getenv.hpp>
#include <boost/asio/ip/network_v4.hpp> // 使用 Boost 库中的 asio 进行网络处理


std::shared_ptr<Config> defaultConfig() {
    std::shared_ptr<EngineConfig> defaultengineConfig;
    boost::optional<myerror> err;
    std::tie(defaultengineConfig, err) = defaultEngineConfig();

    if (err) {
        throw *err;
    };

    defaultengineConfig->SignaturePolicyPath = DefaultSignaturePolicyPath;

    // 注意: 目前我们希望 Windows 使用系统位置。
    // GetRootlessUID 在 Windows 上返回 -1，因此排除负数范围
    if (GetRootlessUID() > 0) {
        boost::filesystem::path configHome="";
        const char* home = boost::compute::detail::getenv("HOME");
        try {
            configHome = boost::filesystem::path(home) / ".config";
        } catch (myerror& e) {
            throw;
        }
        boost::filesystem::path sigPath = configHome / DefaultRootlessSignaturePolicyPath;
        defaultengineConfig->SignaturePolicyPath = sigPath.string();

        if (!boost::filesystem::exists(sigPath)) {
            if (boost::filesystem::exists(DefaultSignaturePolicyPath)) {
                defaultengineConfig->SignaturePolicyPath = DefaultSignaturePolicyPath;
            }
        }
    }
    auto cgroupNS="host";
    bool cgroup2;
    boost::optional<std::error_code> err2;
    std::tie(cgroup2,err2)=Enabled();
    if(cgroup2){
        cgroupNS="private";
    }
    auto ret_config=std::make_shared<Config>();
    auto Containers=std::make_shared<ContainersConfig>();
    auto Network=std::make_shared<NetworkConfig>();
    auto Engine=std::make_shared<EngineConfig>();
    auto Secrets=std::make_shared<SecretConfig>();
    auto Machine=std::make_shared<MachineConfig>();
    auto Farms=std::make_shared<FarmConfig>();
    
    Containers->Annotations=std::make_shared<Slice>();
    Containers->ApparmorProfile=DefaultApparmorProfile;
    Containers->BaseHostsFile="";
    Containers->CgroupNS=cgroupNS;
    Containers->Cgroups="enabled";
    Containers->DNSOptions=std::make_shared<Slice>();
    Containers->DNSSearches=std::make_shared<Slice>();
    Containers->DNSServers=std::make_shared<Slice>();
    Containers->DefaultCapabilities=NewSlice(DefaultCapabilities);
    Containers->DefaultSysctls=std::make_shared<Slice>();
    Containers->DefaultUlimits=NewSlice(getDefaultProcessLimits());
    Containers->Devices=std::make_shared<Slice>();
    Containers->EnableKeyring=true;
    Containers->EnableLabeling=selinuxEnabled();
    Containers->Env=NewSlice(defaultContainerEnv);
    Containers->EnvHost=false;
    Containers->HTTPProxy=true;
    Containers->IPCNS="shareable";
    Containers->Init=false;
    Containers->InitPath="";
    Containers->LogDriver=defaultLogDriver();
    Containers->LogSizeMax=DefaultLogSizeMax;
    Containers->Mounts=std::make_shared<Slice>();
    Containers->NetNS="private";
    Containers->NoHosts=false;
    Containers->PidNS="private";
    Containers->PidsLimit=DefaultPidsLimit;
    Containers->ShmSize=DefaultShmSize;
    Containers->TZ="";
    Containers->UTSNS="private";
    Containers->Umask="0022";
    Containers->UserNSSize=DefaultUserNSSize;
    Containers->Volumes=std::make_shared<Slice>();

    Network->FirewallDriver="";
    Network->DefaultNetwork="podman";
    Network->DefaultSubnet=DefaultSubnet;
    Network->DefaultSubnetPools=DefaultSubnetPools;
    Network->DefaultRootlessNetworkCmd="podman";
    Network->DNSBindPort=0;
    Network->CNIPluginDirs=NewSlice(DefaultCNIPluginDirs);
    Network->NetavarkPluginDirs=NewSlice(DefaultNetavarkPluginDirs);

    Engine=defaultengineConfig;
    Secrets=defaultSecretConfig();
    Machine=defaultMachineConfig();
    Farms=defaultFarmConfig();
    ret_config->Containers=Containers;
    ret_config->Network=Network;
    ret_config->Engine=Engine;
    ret_config->Secrets=Secrets;
    ret_config->Machine=Machine;
    ret_config->Farms=Farms;
    return ret_config;
    // return std::make_shared<Config>();
}
std::shared_ptr<SecretConfig> defaultSecretConfig() {
    auto secret=std::make_shared<SecretConfig>();
    secret->Driver="file";
    return secret;
}
// defaultMachineConfig 返回默认的机器配置
std::shared_ptr<MachineConfig> defaultMachineConfig() {
    // 获取系统 CPU 核心数量
    uint64_t cpus = std::thread::hardware_concurrency() / 2;
    if (cpus == 0) {
        cpus = 1;
    }
    auto mach=std::make_shared<MachineConfig>();
    mach->CPUs=cpus;
    mach->DiskSize=100;
    mach->Memory=2048;
    mach->Image="";
    mach->User=getDefaultMachineUser();
    mach->Volumes=NewSlice(getDefaultMachineVolumes());
    mach->Rosetta=true;
    return mach;
}
std::shared_ptr<FarmConfig> defaultFarmConfig() {
    auto farm=std::make_shared<FarmConfig>();
    farm->List=std::map<std::string,std::string>();
    return farm;
}
std::tuple<std::shared_ptr<EngineConfig>,boost::optional<myerror>> defaultEngineConfig() {
    auto c=std::make_shared<EngineConfig>();
    std::string tmp;
    boost::optional<myerror> err;
    std::tie(tmp,err)=defaultTmpDir();
    if(err){
        return std::make_tuple(nullptr,err);
    }
    c->TmpDir=tmp;
    c->EventsLogFileMaxSize=std::make_shared<eventsLogMaxSize>(DefaultEventsLogSizeMax);
    c->CompatAPIEnforceDockerHub=true;
    c->ComposeProviders=NewSlice(getDefaultComposeProviders());
    c->ComposeWarningLogs=true;
    auto storeOpts=DefaultStoreOptions();
    // if(storeOpts.GraphRoot=""){
        
    // }
    c->graphRoot=storeOpts.graph_root;
    c->ImageCopyTmpDir=getDefaultTmpDir();
    c->VolumePluginTimeout=DefaultVolumePluginTimeout;
    c->CompressionFormat="gzip";
    c->HelperBinariesDir=NewSlice(std::vector<std::string>());
    // if(additionalHelperBinariesDir)
    c->HooksDir=NewSlice(DefaultHooksDirs);
    c->ImageDefaultTransport=_defaultTransport;
    c->ImageVolumeMode=_defaultImageVolumeMode;
    c->ImageDefaultFormat="oci";
    c->CgroupManager=defaultCgroupManager();
    c->ServiceTimeout=uint32_t(5);
    c->StopTimeout=uint32_t(10);
    c->PodmanshTimeout=uint32_t(30);
    c->ExitCommandDelay=uint32_t(5*60);
    c->Remote=isRemote();
    c->Retry=3;
    c->OCIRuntimes=std::map<std::string,std::vector<string>>{
        {"crun",{
			"/usr/bin/crun",
			"/usr/sbin/crun",
			"/usr/local/bin/crun",
			"/usr/local/sbin/crun",
			"/sbin/crun",
			"/bin/crun",
			"/run/current-system/sw/bin/crun"
            }},
        {"crun-vm",{
			"/usr/bin/crun-vm",
			"/usr/local/bin/crun-vm",
			"/usr/local/sbin/crun-vm",
			"/sbin/crun-vm",
			"/bin/crun-vm",
			"/run/current-system/sw/bin/crun-vm"
            }},
        {"runc",{
            "/usr/bin/runc",
            "/usr/sbin/runc",
            "/usr/local/bin/runc",
            "/usr/local/sbin/runc",
            "/sbin/runc",
            "/bin/runc",
            "/run/current-system/sw/bin/runc"
            }},
        {"runj",{
            "/usr/local/bin/runj"
            }},
        {"kata",{
			"/usr/bin/kata-runtime",
			"/usr/sbin/kata-runtime",
			"/usr/local/bin/kata-runtime",
			"/usr/local/sbin/kata-runtime",
			"/sbin/kata-runtime",
			"/bin/kata-runtime",
			"/usr/bin/kata-qemu",
			"/usr/bin/kata-fc"
            }},
        {"runsc",{
			"/usr/bin/runsc",
			"/usr/sbin/runsc",
			"/usr/local/bin/runsc",
			"/usr/local/sbin/runsc",
			"/bin/runsc",
			"/sbin/runsc",
			"/run/current-system/sw/bin/runsc"
		}},
        {"youki", {
			"/usr/local/bin/youki",
			"/usr/bin/youki",
			"/bin/youki",
			"/run/current-system/sw/bin/youki"
		}},
        {"krun",{
			"/usr/bin/krun",
			"/usr/local/bin/krun"
		}},
        {"ocijail",{
			"/usr/local/bin/ocijail"
		}}
    };
    c->PlatformToOCIRuntime=std::map<std::string,std::string>{
        {"wasi/wasm", "crun-wasm"},
        {"wasi/wasm32", "crun-wasm"},
        {"wasi/wasm64", "crun-wasm"}
    };
    // c->OCIRuntime=c->findRuntime();
    c->ConmonEnvVars=NewSlice(std::vector<std::string>{
        "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"});
    c->ConmonPath=NewSlice(std::vector<std::string>{
		"/usr/libexec/podman/conmon",
		"/usr/local/libexec/podman/conmon",
		"/usr/local/lib/podman/conmon",
		"/usr/bin/conmon",
		"/usr/sbin/conmon",
		"/usr/local/bin/conmon",
		"/usr/local/sbin/conmon",
		"/run/current-system/sw/bin/conmon"
    });
    c->PullPolicy=DefaultPullPolicy;
    c->RuntimeSupportsJSON=NewSlice(std::vector<std::string>{
        "crun",
		"runc",
		"kata",
		"runsc",
		"youki",
		"krun",
		"ocijail"});
    c->RuntimeSupportsNoCgroups=NewSlice(std::vector<std::string>{
        "crun",
        "krun"});
    c->RuntimeSupportsKVM=NewSlice(std::vector<std::string>{
        "kata", "kata-runtime", "kata-qemu", "kata-fc", "krun"});
    c->NoPivotRoot=false;
    c->InfraImage=DefaultInfraImage;
    c->EnablePortReservation=true;
    c->NumLocks=2048;
    c->EventsLogger=defaultEventsLogger();
    c->DetachKeys=DefaultDetachKeys;
    c->SDNotify=false;
    c->ChownCopiedFiles=true;
    c->LockType=getDefaultLockType();
    c->ChownCopiedFiles=true;
    c->SSHConfig=getDefaultSSHConfig();
    c->KubeGenerateType="pod";
    return std::make_tuple(c,boost::none);
}

std::tuple<std::string,boost::optional<myerror>> defaultTmpDir(){
    return std::make_tuple("/run/libpod",boost::none);
}
std::vector<std::string> getDefaultComposeProviders(){
    return defaultUnixComposeProviders;
}
std::string getDefaultTmpDir(){
    return defaultRunRoot+"\\tmp";
}
std::string defaultCgroupManager(){
    return CgroupfsCgroupsManager;
}
std::string defaultEventsLogger(){
    return "file";
}
std::string defaultLogDriver(){
    return "k8s-file";
}
std::string getDefaultSSHConfig(){
    #ifdef __linux__
    return "/root/.ssh/config";
    #else
    return "C:\\ProgramData\\ssh\\sshd_config";
    #endif
}
SubnetPool parseSubnetPool(const std::string& subnet, int size) {
    // 使用 Boost 库解析 CIDR
    boost::system::error_code ec;
    boost::asio::ip::network_v4 network = boost::asio::ip::make_network_v4(subnet, ec);
    
    if (ec) {
        throw std::invalid_argument("Invalid subnet provided");
    }

    boost::asio::ip::address_v4 ip = network.address();
    unsigned short prefixLength = network.prefix_length();

    auto ipNet =std::make_shared<IPNet>(ip, prefixLength);
    
    return SubnetPool(ipNet, size);
}
std::string getDefaultMachineUser(){
    return "core";
}