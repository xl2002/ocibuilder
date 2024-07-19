#include "pkg/parse/parse.h"
#include <algorithm>
#include "util/util.h"

const
	// SeccompDefaultPath defines the default seccomp path
	string SeccompDefaultPath = "/usr/share/containers/seccomp.json",
	// SeccompOverridePath if this exists it overrides the default seccomp path
	SeccompOverridePath = "/etc/containers/seccomp.json",
	// TypeBind is the type for mounting host dir
	TypeBind = "bind",
	// TypeTmpfs is the type for mounting tmpfs
	TypeTmpfs = "tmpfs",
	// TypeCache is the type for mounting a common persistent cache from host
	TypeCache = "cache",
	// mount=type=cache must create a persistent directory on host so it's available for all consecutive builds.
	// Lifecycle of following directory will be inherited from how host machine treats temporary directory
	BuildahCacheDir = "buildah-cache";

PullPolicy PullPolicyFromOptions(Command* c){
    Flagset* flags=c->Flags();
    int pullFlagsCount=0;
    if(c->Flag_find("pull")->changed){
        pullFlagsCount++;
    }
    if(c->Flag_find("pull_always")->changed){
        pullFlagsCount++;
    }
    if(c->Flag_find("pull_nerver")->changed){
        pullFlagsCount++;
    }
    if(pullFlagsCount>0){
        throw myerror("can only set one of 'pull' or 'pull-always' or 'pull-never'");
    }
    Pull_Policy pullPolicy=PullIfMissing;
    string pullFlagValue=c->Flag_find("pull")->value->String();
    std::transform(pullFlagValue.begin(), pullFlagValue.end(), pullFlagValue.begin(), ::tolower);
    if(pullFlagValue=="true"||pullFlagValue=="ifnewer"){
        pullPolicy=PullIfNewer;
    }
    try
    {
        bool pullAlwaysFlagValue=flags->GetBool("pull-always");

        if(pullAlwaysFlagValue|| pullFlagValue=="always"){
            pullPolicy=PullAlways;
        }
        bool pullNeverFlagValue=flags->GetBool("pull-never");
        if(pullNeverFlagValue|| pullFlagValue=="nerver"||pullFlagValue=="false"){
            pullPolicy=PullNever;
        }
        std::cout << "Pull Policy for pull [" << static_cast<int>(pullPolicy) << "]" << std::endl;

    }
    catch(const myerror& e)
    {
        throw;
    }
    return PullPolicy(pullPolicy);
}

shared_ptr< SystemContext> SystemContextFromOptions(Command* c){
    Flagset* flags=c->Flags();
    // SystemContext* ctx=new SystemContext();
    auto ctx=make_shared<SystemContext>();
    // auto findFlagFunc=Command::Flag_find;
    try
    {
        string certDir=flags->GetString("cert-dir");
        
        ctx->DockerCertPath=certDir;
        bool tlsVerify= flags->GetBool("tls-verify");
        if(c->Flag_find("tls-verify")->changed){

        }
        bool insecure= flags->GetBool("insecure");
        if(c->Flag_find("insecure")->changed){

        }
        bool disableCompression= flags->GetBool("disable-compression");
        if(disableCompression){
            ctx->OCIAcceptUncompressedLayers=true;
        }else{
            ctx->DirForceCompress=true;
        }
        string creds= flags->GetString("creds");
        if(c->Flag_find("creds")->changed){

        }
        string sigPolicy= flags->GetString("signature-policy");
        if(c->Flag_find("signature-policy")->changed){

        }
        string authfile= flags->GetString("authfile");
        if(c->Flag_find("authfile")->changed){

        }
        string regConf= flags->GetString("registries-conf");
        if(c->Flag_find("registries-conf")->changed){

        }
        string regConfDir= flags->GetString("registries-conf-dir");
        if(c->Flag_find("registries-conf-dir")->changed){

        }
        string shortNameAliasConf= flags->GetString("short-name-alias-conf");
        if(c->Flag_find("short-name-alias-conf")->changed){

        }
        ctx->DockerRegistryUserAgent="Buildah/"+Version;
        if(c->Flag_find("os")!=nullptr && c->Flag_find("os")->changed){

        }
        if(c->Flag_find("arch")!=nullptr && c->Flag_find("arch")->changed){
            
        }
        if(c->Flag_find("variant")!=nullptr && c->Flag_find("variant")->changed){
            
        }
        if(c->Flag_find("platform")!=nullptr && c->Flag_find("platform")->changed){
            
        }
        ctx->BigFilesTemporaryDir=GetTempDir();
    }
    catch(const myerror& e)
    {
        throw;
    }
    
    return ctx;
}
std::string GetTempDir() {
    // try {
    //     std::string tmpdir = GetEnvVar("TMPDIR");
    //     if (!tmpdir.empty()) {
    //         try {
    //             return GetAbsolutePath(tmpdir);
    //         } catch (const std::exception &e) {
    //             std::cerr << "Ignoring TMPDIR from environment, evaluating it: " << e.what() << std::endl;
    //         }
    //     }

    //     std::shared_ptr<Config> containerConfig = Config::Default();
    //     try {
    //         return containerConfig->ImageCopyTmpDir();
    //     } catch (const std::exception &e) {
    //         std::cerr << "Error getting ImageCopyTmpDir: " << e.what() << std::endl;
    //     }
    // } catch (const std::exception &e) {
    //     std::cerr << "Error getting default config: " << e.what() << std::endl;
    // }

    return "D:/tmp";
}

shared_ptr<Isolation> IsolationOption(string isolation){
    if(!isolation.empty()){
        transform(isolation.begin(),isolation.end(),isolation.begin(),::tolower);
        if (isolation == "oci" || isolation == "default") {
            return make_shared<Isolation>(IsolationOCI);
        } else if (isolation == "rootless") {
            return make_shared<Isolation>(IsolationOCIRootless);
        } else if (isolation == "chroot") {
            return make_shared<Isolation>(IsolationChroot);
        } else {
            throw std::invalid_argument("unrecognized isolation type: " + isolation);
        }
    }else{
        throw std::invalid_argument("isolation type cannot be empty");
    }
}

shared_ptr<CommonBuildOptions> CommonbuildOptions(Command* cmd){
    auto flags=cmd->Flags();
    int64_t memoryLimit;
    int64_t memorySwap;
    bool noDNS;
    auto memVal=flags->GetString("memory");
    if(!memVal.empty()){

    }
    auto memSwapValue=flags->GetString("memory-swap");
    if(!memSwapValue.empty()){

    }
    auto noHostname= flags->GetBool("no-hostname");
    auto noHosts= flags->GetBool("no-hosts");
    auto addHost=flags->GetStringArray("add-host");
    if(!addHost.empty()){

    }
    auto noDNS = false;
	auto dnsServers = vector<string>();
    if(flags->Changed("dns")){

    }
    auto dnsSearch = vector<string>();
    if(flags->Changed("dns-search")){

    }
    auto dnsOptions = vector<string>();
    if(flags->Changed("dns-option")){

    }
    auto volumes= flags->GetStringArray("volume");
    auto cpuPeriod= flags->GetUint64("cpu-period");
    auto cpuQuota= flags->GetInt64("cpu-quota");
    auto cpuShares= flags->GetUint64("cpu-shares");
    auto httpProxy= flags->GetBool("http-proxy");
    auto identityLabel= flags->GetBool("identity-label");
    auto omitHistory= flags->GetBool("omit-history");
    auto ulimit=vector<string>();
    if(flags->Changed("ulimit")){

    }
    auto secrets= flags->GetStringArray("secret");
    auto sshsources= flags->GetStringArray("ssh");
    auto ociHooks= flags->GetStringArray("hooks-dir");
    auto commonOpts=make_shared<CommonBuildOptions>();

    commonOpts->AddHost=addHost;
    commonOpts->CPUPeriod=cpuPeriod;
    commonOpts->CPUQuota=cpuQuota;
    commonOpts->CPUSetCPUs=cmd->Flag_find("cpuset-cpus")->value->String();
    commonOpts->CPUSetMems=cmd->Flag_find("cpuset-mems")->value->String();
    commonOpts->CPUShares=cpuShares;
    commonOpts->CgroupParent=cmd->Flag_find("cgroup-parent")->value->String();
    commonOpts->DNSOptions=dnsOptions;
    commonOpts->DNSSearch=dnsSearch;
    commonOpts->DNSServers=dnsServers;
    commonOpts->HTTPProxy=httpProxy;
    commonOpts->IdentityLabel=static_cast<int8_t>(NewOptionalBool(identityLabel));
    commonOpts->Memory=memoryLimit;
    commonOpts->MemorySwap=memorySwap;
    commonOpts->NoHostname=noHostname;
    commonOpts->OmitHistory=omitHistory;
    // commonOpts->ShmSize=cmd->Flag_find("shm-size")->value->String();
    commonOpts->Ulimit=ulimit;
    commonOpts->Volumes=volumes;
    commonOpts->Secrets=secrets;
    commonOpts->SSHSources=sshsources;
    commonOpts->OCIHooksDir=ociHooks;
    auto securityOpts=flags->GetStringArray("security-opt");
    try
    {
        parseSecurityOpts(securityOpts,commonOpts);
    }
    catch(const myerror& e)
    {
        throw;
    }
    return commonOpts;
}

void parseSecurityOpts(vector<string> securityOpts,shared_ptr<CommonBuildOptions> commonOpts){
    for(auto it:securityOpts){
        
    }
    if(commonOpts->SeccompProfilePath==""){
        if(fileExists(SeccompOverridePath)){
            commonOpts->SeccompProfilePath = SeccompOverridePath;
        }else{
            if(!fileExists(SeccompDefaultPath)){

            }else{
                commonOpts->SeccompProfilePath = SeccompDefaultPath;
            }
        }
    }
}

shared_ptr<NamespaceOptions> Namespaceoptions(Command* cmd,NetworkConfigurationPolicy& networkPolicy){
    auto flags=cmd->Flags();
    auto options=make_shared<NamespaceOptions>();
    auto policy=NetworkDefault;
    for(auto it : vector<string>()){

    }
    networkPolicy=policy;
    return options;
}

shared_ptr<NamespaceOptions> idmappingOptions(Command* cmd,shared_ptr<Isolation> isolation,shared_ptr<IDMappingOptions>idmapOptions){
    auto flags=cmd->Flags();
    auto persistentFlags=cmd->PersistentFlags();
    bool isAuto=false;
    auto autoOpts=make_shared<AutoUserNsOptions>();
    auto user=cmd->Flag_find("userns-uid-map-user")->value->String();
    auto user=cmd->Flag_find("userns-gid-map-group")->value->String();
}


