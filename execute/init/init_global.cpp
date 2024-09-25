#include "init/init_global.h"


//全局变量的定义
std::unordered_map<std::string, std::function<std::shared_ptr<Driver>(const std::string&, const driver_Options&)>> drivers;
// 定义错误信息
std::string ErrNotSupported;
std::string ErrPrerequisites;
std::string ErrIncompatibleFS;
std::string ErrLayerUnknown;
std::shared_ptr<PolicyTransportScopes> storageAllowedPolicyScopes;
bool isCgroupV2;
boost::optional<std::error_code> isCgroupV2Err;
Flagset* CommandLine;
std::string _configPath;
std::string UserOverrideContainersConfig;
std::string bindirPrefix;

std::string OverrideContainersConfig;
std::string DefaultContainersConfig;
std::string DefaultSignaturePolicyPath;

map<string,Pull_Policy>PolicyMap;

Algorithm_sha256 SHA256;
Algorithm_sha256 Canonical_sha256;
std::map<Algorithm_sha256, std::regex> anchoredEncodedRegexps;

std::map<std::string,bool> replaceEnvAllowed;
std::map<std::string,bool> allowWordExpansion;

std::shared_ptr<BufferPool_interface> globalBufferPool;

std::string logrusPackage;
int minimumCallerDepth;
int maximumCallerDepth;
int knownLogrusFrames;

std::shared_ptr<Logger> Std;

Command rootcmd;                         ///<声明全局根命令
std::shared_ptr<Config> defaultContainerConfig;    ///<声明全局镜像config
int exitcode;                            ///<声明全局程序结束标志
globalFlags globalFlagOptions;           ///<声明全局标签选项

std::shared_ptr<Regexp> tokenEscapeCommand;
std::shared_ptr<Regexp> tokenPlatformCommand;
std::shared_ptr<Regexp> tokenWhitespace;
std::shared_ptr<Regexp> tokenComment;
std::map<std::string, bool> heredocDirectives;
std::string defaultPlatformToken;
char DefaultEscapeToken;
std::map<std::string, bool> heredocCompoundDirectives;
namespace dockerfilecommand {
    std::string Add;
    std::string Arg;
    std::string Cmd;
    std::string Copy;
    std::string Entrypoint;
    std::string Env;
    std::string Expose;
    std::string From;
    std::string Healthcheck;
    std::string Label;
    std::string Maintainer;
    std::string Onbuild;
    std::string Run;
    std::string Shell;
    std::string StopSignal;
    std::string User;
    std::string Volume;
    std::string Workdir;
};
bool precompile;
std::shared_ptr<StoreTransport_interface> Transport;
std::string overlayDriver;
std::string overlay2;
std::string storageConfEnv;
Descriptor DescriptorEmptyJSON;




std::once_flag flag_global;
void init_global(){
    drivers={};
    ErrNotSupported = "driver not supported";
    ErrPrerequisites = "prerequisites for driver not satisfied (wrong filesystem?)";
    ErrIncompatibleFS = "backing file system is unsupported for this graph driver";
    ErrLayerUnknown = "unknown layer";
    storageAllowedPolicyScopes=std::make_shared<PolicyTransportScopes>();
    isCgroupV2=false;
    isCgroupV2Err=std::error_code();
    CommandLine=NewFlagSet("buildah");
    _configPath = "containers/containers.conf";
	// UserOverrideContainersConfig holds the containers config path overridden by the rootless user
	UserOverrideContainersConfig = ".config/" + _configPath;
	// Token prefix for looking for helper binary under $BINDIR
	bindirPrefix = "$BINDIR";
    
	// OverrideContainersConfig holds the default config path overridden by the root user
	OverrideContainersConfig = "/etc/";

	// DefaultContainersConfig holds the default containers config path
    DefaultContainersConfig = "/usr/share/" + _configPath;
	// DefaultSignaturePolicyPath is the default value for the
	// policy.json file.
    DefaultSignaturePolicyPath = "D:\\oci_images\\policy.json";
    PolicyMap ={
        {"missing",PullIfMissing},
        {"always", PullAlways},
        {"never",  PullNever},
        {"ifnewer", PullIfNewer}
    };
    SHA256=Algorithm_sha256("sha256");
    Canonical_sha256=SHA256;
    anchoredEncodedRegexps={{ SHA256, std::regex("^[a-f0-9]{64}$") }};
    replaceEnvAllowed={
        {"env",true},
        {"arg",true},
        {"volume",true},
        {"label",true},
        {"add",true},
        {"workdir",true},
        {"expose",true},
        {"user",true},
        {"stopsignal",true},
        {"copy",true},
    };
    allowWordExpansion={{"expose",true}};
    globalBufferPool = std::make_shared<SimpleBufferPool>();
    logrusPackage="";
    minimumCallerDepth=1;
    maximumCallerDepth=25;
    knownLogrusFrames=4;
    Std=New();
    rootcmd=init_rootcmd(); ///<定义rootcmd，作为全局变量
    defaultContainerConfig=nullptr;
    globalFlagOptions={};
    exitcode=0;

    tokenEscapeCommand=Delayed(R"(^#[ \t]*escape[ \t]*=[ \t]*(?P<escapechar>.).*$)");
    tokenPlatformCommand=Delayed(R"(^#[ \t]*platform[ \t]*=[ \t]*(?P<platform>.*)$)");
    tokenWhitespace=Delayed(R"([\t\v\f\r ]+)");
    tokenComment=Delayed(R"(^#.*$)");
    heredocDirectives = {
        {"ADD", true},
        {"COPY", true},
        {"RUN", true}
    };
    // 默认平台标识符，假设为构建的默认平台
    defaultPlatformToken = "windows"; // 用实际的默认平台替代
    DefaultEscapeToken = '\\';
    // 定义允许包含指令的指令集合
    heredocCompoundDirectives = {
        {"ONBUILD", true}
    };
    dockerfilecommand::Add = "add";
    dockerfilecommand::Arg = "arg";
    dockerfilecommand::Cmd = "cmd";
    dockerfilecommand::Copy = "copy";
    dockerfilecommand::Entrypoint = "entrypoint";
    dockerfilecommand::Env = "env";
    dockerfilecommand::Expose = "expose";
    dockerfilecommand::From = "from";
    dockerfilecommand::Healthcheck = "healthcheck";
    dockerfilecommand::Label = "label";
    dockerfilecommand::Maintainer = "maintainer";
    dockerfilecommand::Onbuild = "onbuild";
    dockerfilecommand::Run = "run";
    dockerfilecommand::Shell = "shell";
    dockerfilecommand::StopSignal = "stopsignal";
    dockerfilecommand::User = "user";
    dockerfilecommand::Volume = "volume";
    dockerfilecommand::Workdir = "workdir";
    bool precompile=false;
    Transport= std::make_shared<storageTransport>();
    overlayDriver  = "overlay";
    overlay2       = "overlay2";
    storageConfEnv = "CONTAINERS_STORAGE_CONF";
    DescriptorEmptyJSON = {
        MediaTypeEmptyJSON,
        "sha256:44136fa355b3678a1146ad16f7e8649e94fb4fc21fe77e8310c060f61caaff8a",
        2,
        std::vector<byte>{'{','}'}
    };

}
void initialize_global(){
    std::call_once(flag_global, init_global);
}
