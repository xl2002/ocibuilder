#include "utils/parse/parse.h"
#include <algorithm>
#include "filesys/system/specs/specs.h"
#include "utils/common/go/file.h"
#include "cmd/build/imagebuilder/builder.h"
#include "utils/common/error.h"
#include "utils/logger/logrus/exported.h"
#include <boost/filesystem.hpp>
#include "config/defaut.h"
#include "storage/types/options.h"
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
    // if(c->Flag_find("pull_always")->changed){
    //     pullFlagsCount++;
    // }
    // if(c->Flag_find("pull_nerver")->changed){
    //     pullFlagsCount++;
    // }
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
        // Debugf("Pull Policy for pull [%d]",static_cast<int>(pullPolicy));
        // std::cout << "Pull Policy for pull [" << static_cast<int>(pullPolicy) << "]" << std::endl;

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
        ctx->DockerRegistryUserAgent="Buildah/"+version;
        if(c->Flag_find("os")!=nullptr && c->Flag_find("os")->changed){
            ctx->OSChoice=flags->GetString("os");
        }
        if(c->Flag_find("arch")!=nullptr && c->Flag_find("arch")->changed){
            ctx->ArchitectureChoice=flags->GetString("arch");
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

    return defaultGraphRoot+"/tmp";
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
    commonOpts->IdentityLabel=NewOptionalBool(identityLabel);
    commonOpts->Memory=memoryLimit;
    commonOpts->MemorySwap=memorySwap;
    commonOpts->NoHostname=noHostname;
    commonOpts->OmitHistory=omitHistory;
    commonOpts->ShmSize=cmd->Flag_find("shm-size")->value->String();
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
                commonOpts->SeccompProfilePath = SeccompOverridePath;
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
shared_ptr<NamespaceOptions> Namespaceoptions(Command* cmd,std::shared_ptr<NetworkConfigurationPolicy> networkPolicy){
    auto flags=cmd->Flags();
    auto options=make_shared<NamespaceOptions>();
    auto policy=NetworkDefault;
    for(auto it : vector<string>()){

    }
    networkPolicy->value=policy;
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
    //         throw myerror("mapping " + s + " is not in the form containerid:hostid:size[,...]");
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
    idm.AutoUserNsOpts=autoOpts;
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
        os=cmd->Flags()->GetString("os");
    }
    if(cmd->Flag_find("arch")->changed){
        arch=cmd->Flags()->GetString("arch");
    }
    if(cmd->Flag_find("variant")->changed){
        variant=cmd->Flags()->GetString("variant");
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
/**
 * @brief 解析单个volume字符串并返回Mount对象
 * 
 * @param volume 格式为"src:dest[:options]"的volume字符串
 * @return Mount 解析后的挂载点对象
 * @throws myerror 如果volume格式无效
 */
Mount Volume(string volume){
    auto mount=Mount();
    
    return mount;
}

/**
 * @brief 批量解析volume字符串
 * 
 * @param volumes 包含多个volume字符串的vector
 * @throws myerror 如果任何volume字符串格式无效
 */
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


/**
 * @brief 查找并解析容器忽略文件
 * 
 * @param contextDir 上下文目录路径
 * @param path 显式指定的忽略文件路径(可选)
 * @param containerFiles 容器文件列表
 * @return tuple<vector<string>, string> 返回排除模式列表和忽略文件路径
 * @throws myerror 如果解析忽略文件时出错
 */
std::tuple<std::vector<std::string>, std::string> ContainerIgnoreFile(
    const std::string& contextDir, const std::string& path, const std::vector<std::string>& containerFiles) {
    
    std::vector<std::string> excludes;
    std::string ignorePath;

    try {
        if (!path.empty()) {
            excludes = ParseIgnore(path);
            ignorePath = path;
        } else {
            for (const std::string& containerfile : containerFiles) {
                std::string containerfilePath = containerfile;
                #ifndef __linux__
                boost::filesystem::path p(containerfile);
                if(containerfilePath.empty() || !p.is_absolute()){
                    containerfilePath = contextDir + "/" + containerfile;
                }
                #else
                if (containerfilePath.empty() || containerfilePath[0] != '/') {
                    containerfilePath = contextDir + "/" + containerfile;
                }
                #endif
                std::string containerfileIgnore;
                if (fileExists(containerfilePath + ".containerignore")) {
                    containerfileIgnore = containerfilePath + ".containerignore";
                } else if (fileExists(containerfilePath + ".dockerignore")) {
                    containerfileIgnore = containerfilePath + ".dockerignore";
                }

                if (!containerfileIgnore.empty()) {
                    excludes = ParseIgnore(containerfileIgnore);
                    ignorePath = containerfileIgnore;
                    break;
                }
            }

            if (ignorePath.empty()) {
                std::string defaultIgnorePath = contextDir + "/.containerignore";
                if (!fileExists(defaultIgnorePath)) {
                    defaultIgnorePath = contextDir + "/.dockerignore";
                }

                if (fileExists(defaultIgnorePath)) {
                    excludes = ParseIgnore(defaultIgnorePath);
                    ignorePath = defaultIgnorePath;
                }
            }
        }
    } catch (const myerror& e) {
        throw;
        // return std::make_tuple(excludes, ignorePath, new myerror(e.what()));
    }

    return std::make_tuple(excludes, ignorePath);
}

/**
 * @brief 解析并创建Secret对象映射
 * 
 * @param secrets 包含secret配置字符串的vector
 * @return map<string, shared_ptr<Secret>> 返回secret名称到Secret对象的映射
 */
std::map<std::string,std::shared_ptr<Secret>> Secrets(std::vector<std::string> secrets){
    return std::map<std::string,std::shared_ptr<Secret>>();
}

// std::map<std::string,std::shared_ptr<Source>> SSH(std::vector<std::string> sshSources){
// }
