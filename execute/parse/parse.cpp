#include "parse/parse.h"
#include <algorithm>
#include "specs/specs.h"
#include "go/file.h"
// 定义常量字符串

// SeccompDefaultPath 定义了默认的 seccomp 配置文件路径
const string SeccompDefaultPath = "/usr/share/containers/seccomp.json",

// SeccompOverridePath 如果存在，则覆盖默认的 seccomp 配置文件路径
SeccompOverridePath = "/etc/containers/seccomp.json",

// TypeBind 是用于挂载主机目录的类型
TypeBind = "bind",

// TypeTmpfs 是用于挂载 tmpfs 的类型
TypeTmpfs = "tmpfs",

// TypeCache 是用于挂载主机上常用的持久缓存的类型
TypeCache = "cache",

// mount=type=cache 必须在主机上创建一个持久目录，以便在所有连续的构建中都可用。
// 以下目录的生命周期将继承主机如何处理临时目录的方式
BuildahCacheDir = "buildah-cache";

/**
 * @brief 根据命令的标志来确定 pull 策略
 * 
 * @param c 命令对象
 * @return PullPolicy pull 策略
 * @throws myerror 如果标志之间存在冲突，将抛出错误信息
 */
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

/**
 * @brief 根据命令的标志来生成SystemContext
 *
 * @param c 命令对象
 * @return shared_ptr<SystemContext> SystemContext对象
 * @throws myerror 如果标志之间存在冲突，将抛出错误信息
 */
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
        // bool insecure= flags->GetBool("insecure");
        // if(c->Flag_find("insecure")->changed){

        // }
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
/**
 * @brief 获取临时目录
 * @return string 临时目录的路径
 */
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

/**
 * @brief 将字符串表示的Isolation类型转换为Isolation枚举
 * @param isolation 字符串表示的Isolation类型
 * @return shared_ptr<Isolation> Isolation枚举
 */
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

/**
 * @brief 将命令的标志转换为CommonBuildOptions对象
 * @param cmd 命令指针
 * @return shared_ptr<CommonBuildOptions> CommonBuildOptions对象
 */
shared_ptr<CommonBuildOptions> CommonbuildOptions(Command* cmd){
    auto flags=cmd->Flags();
    int64_t memoryLimit=0;
    int64_t memorySwap=0;
    bool noDNS=false;
    auto memVal=flags->GetString("memory");
    if(!memVal.empty()){

    }
    auto memSwapValue=flags->GetString("memory-swap");
    if(!memSwapValue.empty()){

    }
    auto noHostname= flags->GetBool("no-hostname");
    auto noHosts= flags->GetBool("no-hosts");
    auto addHost=flags->GetStringSlice("add-host");
    if(!addHost.empty()){

    }
    noDNS = false;
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

/**
 * @brief 解析安全选项
 * @details 解析传入的安全选项，并将解析结果设置到CommonBuildOptions中
 * @param securityOpts 安全选项的字符串数组
 * @param commonOpts CommonBuildOptions的指针
 * @throws 无
 */
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

/**
 * @brief 该函数将Command指针cmd和NetworkConfigurationPolicy的引用networkPolicy作为参数
 *        并将Command的Flags()结果转换为NamespaceOptions智能指针
 * @param cmd Command指针
 * @param networkPolicy NetworkConfigurationPolicy的引用
 * @return NamespaceOptions智能指针
 */
shared_ptr<NamespaceOptions> Namespaceoptions(Command* cmd,NetworkConfigurationPolicy& networkPolicy){
    auto flags=cmd->Flags();
    auto options=make_shared<NamespaceOptions>();
    auto policy=NetworkDefault;
    for(auto it : vector<string>()){

    }
    networkPolicy=policy;
    //对networkPolicy类型进行判断
    return options;
}

/**
 * @brief 将ID映射的字符串数组转换为std::vector<std::array<uint32_t, 3>>对象
 *
 * @param spec ID映射的字符串数组
 *
 * @return std::vector<std::array<uint32_t, 3>>对象
 */
std::vector<std::array<uint32_t, 3>> parseIDMap(const std::vector<std::string>& spec) {
    std::vector<std::array<uint32_t, 3>> m;
    m.reserve(spec.size());
    // for (const auto& s : spec)
    // {
    //     std::istringstream iss(s);
    //     std::vector<std::string> args{std::istream_iterator<std::string>(iss), {}};
    //     if (args.size() % 3 != 0)
    //     {
    //         throw std::runtime_error("mapping " + s + " is not in the form containerid:hostid:size[,...]");
    //     }
    //     for (size_t i = 0; i < args.size(); i += 3)
    //     {
    //         uint32_t cid, hostid, size;
    //         iss >> cid >> hostid >> size;
    //         m.emplace_back(cid, hostid, size);
    //     }
    // }
    return m;
}

/**
 * @brief A function to generate NamespaceOptions based on input parameters.
 *
 * @param cmd Command pointer for accessing flags and persistent flags
 * @param isolation Isolation options for namespace isolation
 * @param idmapOptions IDMapping options for mapping user and group IDs
 *
 * @return shared_ptr<NamespaceOptions> representing the generated NamespaceOptions
 *
 * @throws None
 */
shared_ptr<NamespaceOptions> idmappingOptions(Command* cmd,shared_ptr<Isolation> isolation,shared_ptr<IDMappingOptions> idmapOptions)
{
    // Get the flags and persistent flags from the command
    auto flags=cmd->Flags();
    auto persistentFlags=cmd->PersistentFlags();

    // Initialize a boolean flag to indicate whether auto-mapping is enabled
    bool isAuto=false;

    // Create an instance of AutoUserNsOptions
    auto autoOpts=make_shared<AutoUserNsOptions>();

    // Get the values of the userns-uid-map-user and userns-gid-map-group flags
    // from the command flags
    auto user=cmd->Flag_find("userns-uid-map-user")->value->String();
    auto group=cmd->Flag_find("userns-gid-map-group")->value->String();
    // 如果只指定了用户或组，则使用相同的值作为另一个，因为我们需要同时指定才能使用名称初始化映射。
    if (user.empty() && !group.empty()) {
        user = group;
    }
    if (group.empty() && !user.empty()) {
        group = user;
    }
    // 要么从空开始，要么从名称开始。
    auto mapping=NewIDMappingsFromMaps(vector<IDMap>(),vector<IDMap>());
    if(user!="" && group!=""){

    }
    auto globalOptions=persistentFlags;
        // We'll parse the UID and GID mapping options the same way.
    auto buildIDMap = [=](const std::vector<IDMap>& basemap, const std::string& option) {
        std::vector<LinuxIDMapping> outmap;
        outmap.reserve(basemap.size());
        // Start with the name-based map entries.
        for (const auto& m : basemap) {

            outmap.push_back(LinuxIDMapping{
                static_cast<uint32_t>(m.ContainerID),
                static_cast<uint32_t>(m.HostID),
                static_cast<uint32_t>(m.Size),
            });
        }
        // Parse the flag's value as one or more triples (if it's even
        // been set), and append them.
        std::vector<std::string> spec;
        if(globalOptions->Lookup(option)!=nullptr && globalOptions->Lookup(option)->changed){
            spec=globalOptions->GetStringArray(option);
        }
        if (cmd->Flag_find(option)->changed) {
            spec = flags->GetStringArray(option);
        }
        std::vector<std::array<uint32_t,3>> idmap = parseIDMap(spec);
        for (const auto& m : idmap) {
            outmap.push_back(LinuxIDMapping{
                m[0],
                m[1],
                m[2],
            });
        }
        return outmap;
    };
    auto uidmap=buildIDMap(mapping->UIDs(), "userns-uid-map");
    auto gidmap=buildIDMap(mapping->GIDs(), "userns-gid-map");
    if(uidmap.size()==0 && gidmap.size()!=0){
        uidmap=gidmap;
    }
    if(gidmap.size()==0 && uidmap.size()!=0){
        gidmap=uidmap;
    }
    auto usernsOption=NamespaceOption();
    usernsOption.Name=UserNamespace;
    usernsOption.Host=uidmap.size()==0 && gidmap.size()==0;
    if(cmd->Flag_find("userns")->changed){

    }
    auto usernsOptions=NamespaceOptions();
    usernsOptions.val.emplace_back(usernsOption);
    if ((!uidmap.empty() || !gidmap.empty()) && usernsOption.Host) {
        throw myerror("can not specify ID mappings while using host's user namespace");
    }
    auto idm=IDMappingOptions();
    idm.HostUIDMapping=usernsOption.Host;
    idm.HostGIDMapping=usernsOption.Host;
    idm.UIDMap=uidmap;
    idm.GIDMap=gidmap;
    idm.AutoUserNs=isAuto;
    idm.AutoUserNsOpts=*autoOpts;
    idmapOptions=make_shared<IDMappingOptions>(idm);
    return make_shared<NamespaceOptions>(usernsOptions);
}

/**
 * @brief 从命令行标志中获取平台列表
 * 
 * @param cmd 命令对象
 * @return std::vector<platforms> 平台列表
 * @throws myerror 如果标志之间存在冲突，将抛出错误信息
 */
std::vector<platforms> PlatformsFromOptions(Command* cmd){
    std::string os,arch,variant;
    if(cmd->Flag_find("os")->changed){

    }
    if(cmd->Flag_find("arch")->changed){

    }
    if(cmd->Flag_find("variant")->changed){

    }
    auto Platforms=std::vector<platforms>();
    auto p=platforms();
    p.OS=os;
    p.Arch=arch;
    p.Variant=variant;
    Platforms.emplace_back(p);
    if(cmd->Flag_find("platform")->changed){

    }
    return Platforms;
}
// Volume parses the input of --volume
Mount Volume(string volume){
    auto mount=Mount();
    
    return mount;
}

void Volumes( std::vector<std::string> volumes){
    try
    {
        if(volumes.empty()){
            return;
        }
        for(auto& v:volumes){
            Volume(v);
        }
    }
    catch(const myerror& e)
    {
        throw;
    }
    
    
}