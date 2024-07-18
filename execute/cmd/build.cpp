/**
 * @file build.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 build.h头文件中定义的方法进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "cmd/build.h"

/**
 * @brief 返回build命令的标志
 * <p>根据BuildOptions结构体中的标志名，创建并初始化每个flag
 * 
 * @param br br为BuildOptions的对象，存储build命令的flag
 * @return Flagset* 返回一个标志集指针，注意是动态分配，不就随着函数的结束而销毁
 */
Flagset* Getbuildflags(BudResults* br){
    //动态分配分配一个Flagset指针，用于存储初始化中的flag，并作为返回值
    Flagset* flags=new Flagset();
    //调用flag初始化函数
    flags->BoolVar(br->allplatform,"all-platforms",false,"attempt to build for all base image platforms");
    flags->String("arch","amd64","set the ARCH of the image to the provided value instead of the architecture of the host");
    flags->StringArrayVar(br->annotation,"annotation",vector<string>(),"set metadata for an image (default [])");
    flags->StringArrayVar(br->tag,"tag",vector<string>(),"tagged `name` to apply to the built image");
    flags->StringVar(br->osversion,"os-version","","set required OS `version` for the target image instead of the value from the base image");
    flags->StringVar(br->Authfile, "authfile", "", "path of the authentication file.");
    flags->StringArrayVar(br->BuildArg,"build-arg", vector<string>(), "`argument=value` to supply to the builder");
    flags->StringArrayVar(br->CacheFrom,"cache-from", vector<string>(), "remote repository list to utilise as potential cache source.");
    flags->StringArrayVar(br->CacheTo,"cache-to", vector<string>(), "remote repository list to utilise as potential cache destination.");
    flags->StringVar(br->CacheTTL,"cache-ttl", "", "only consider cache images under specified duration.");
    flags->StringVar(br->CertDir,"cert-dir", "", "use certificates at the specified path to access the registry");
    flags->BoolVar(br->Compress,"compress", false, "this is a legacy option, which has no effect on the image");
    flags->StringArrayVar(br->File,"file", vector<string>(), "`pathname or URL` of a Dockerfile");
    flags->StringVar(br->Creds, "creds", "", "use `[username[:password]]` for accessing the registry");
	flags->StringVar(br->CWOptions, "cw", "", "confidential workload `options`");
	flags->BoolVar(br->DisableCompression, "disable-compression", true, "don't compress layers by default");
	flags->BoolVar(br->DisableContentTrust, "disable-content-trust", false, "this is a Docker specific option and is a NOOP");
	flags->StringArrayVar(br->envs, "env", vector<string>(), "set environment variable for the image");

    flags->StringVar(br->Format,"format", "oci", "`format` of the built image's manifest and metadata.");
    flags->StringVar(br->From,"from", "", "image name used to replace the value in the first FROM instruction in the Containerfile");
    flags->StringVar(br->Iidfile,"iidfile", "", "`file` to write the image ID to");
    flags->IntVar(br->Jobs, "jobs", 1, "how many stages to run in parallel");
    flags->StringArrayVar(br->Label,"label", vector<string>(), "set metadata for an image (default [])");

    flags->StringArrayVar(br->LayerLabel, "layer-label", vector<string>(), "set metadata for an intermediate image (default [])");
    flags->StringVar(br->Logfile, "logfile", "", "log to `file` instead of stdout/stderr");
    flags->StringVar(br->Manifest, "manifest", "", "add the image to the specified manifest list. Creates manifest list if it does not exist");
    flags->BoolVar(br->NoCache, "no-cache", false, "do not use existing cached images for the container build. Build from the start with a new set of cached layers.");
    flags->Int64Var(br->Timestamp, "timestamp", 0, "set created timestamp to the specified epoch seconds to allow for deterministic builds, defaults to current time");
    
    flags->BoolVar(br->OmitHistory, "omit-history", false, "omit build history information from built image");
    flags->StringArrayVar(br->OCIHooksDir, "hooks-dir", vector<string>(), "set the OCI hooks directory path (may be set multiple times)");
    flags->StringVar(br->Pull, "pull", "true", "pull base and SBOM scanner images from the registry if newer or not present in store, if false, only pull base and SBOM scanner images if not present, if always, pull base and SBOM scanner images even if the named images are present in store, if never, only use images present in store if available");
    flags->Lookup("pull")->NoOptDefVal="true";//allow `--pull ` to be set to `true` as expected.
    flags->BoolVar(br->PullAlways, "pull-always", false, "pull the image even if the named image is present in store");
    flags->MarkHidden("pull-always");
    flags->BoolVar(br->PullNever, "pull-never", false, "do not pull the image, use the image present in store if available");
    flags->MarkHidden("pull-never");
    flags->BoolVar(br->Quiet, "quiet", false, "refrain from announcing build instructions and image read/write progress");
    flags->BoolVar(br->IdentityLabel, "identity-label", true, "add default identity label");
    flags->BoolVar(br->Rm, "rm", true, "remove intermediate containers after a successful build");
    flags->BoolVar(br->Stdin, "stdin", false, "pass stdin into containers");
    flags->StringVar(br->BuildOutput, "output", "", "output destination (format: type=local,dest=path)");
    flags->StringVar(br->Target, "target", "", "set the target build stage to build");
    flags->StringArrayVar(br->OSFeatures,"os-feature", vector<string>(), "set required OS `feature` for the target image in addition to values from the base image");
    
    
    
    return flags;
}

/**
 * @brief 返回图层的通用标志
 * 
 * @param lr 
 * @return Flagset 
 */
Flagset* GetLayerFlags(LayerResults* lr){
    Flagset* flags=new Flagset();
	flags->BoolVar(lr->ForceRm, "force-rm", false, "always remove intermediate containers after a build, even if the build is unsuccessful.");
	flags->BoolVar(lr->Layers, "layers", true, "use intermediate layers during build. Use BUILDAH_LAYERS environment variable to override.");
    return flags;
}
Flagset* GetFromAndBudFlags(FromAndBudResults* fr){
    Flagset* flags=new Flagset();
    flags->StringSliceVar(fr->AddHost, "add-host", vector<string>(), "add a custom host-to-IP mapping (`host:ip`) (default [])");
	flags->StringVar(fr->BlobCache, "blob-cache", "", "assume image blobs in the specified directory will be available for pushing");
	flags->MarkHidden("blob-cache"); 
	flags->StringSliceVar(fr->CapAdd, "cap-add", vector<string>(), "add the specified capability when running (default [])");
	flags->StringSliceVar(fr->CapDrop, "cap-drop", vector<string>(), "drop the specified capability when running (default [])");
	flags->StringVar(fr->CDIConfigDir, "cdi-config-dir", "", "`directory` of CDI configuration files");
	flags->MarkHidden("cdi-config-dir");
	flags->StringVar(fr->CgroupParent, "cgroup-parent", "", "optional parent cgroup for the container");
	flags->Uint64Var(fr->CPUPeriod, "cpu-period", 0, "limit the CPU CFS (Completely Fair Scheduler) period");
	flags->Int64Var(fr->CPUQuota, "cpu-quota", 0, "limit the CPU CFS (Completely Fair Scheduler) quota");
	flags->Uint64Var(fr->CPUShares, "cpu-shares", 0, "CPU shares (relative weight)");
	flags->StringVar(fr->CPUSetCPUs, "cpuset-cpus", "", "CPUs in which to allow execution (0-3, 0,1)");
	flags->StringVar(fr->CPUSetMems, "cpuset-mems", "", "memory nodes (MEMs) in which to allow execution (0-3, 0,1). Only effective on NUMA systems.");
	flags->StringSliceVar(fr->DecryptionKeys, "decryption-key", vector<string>(), "key needed to decrypt the image");
	// flags->StringArrayVar(fr->Devices, "device", defaultContainerConfig.Containers.Devices.Get(), "additional devices to provide");
	// flags->StringSliceVar(fr->DNSSearch, "dns-search", defaultContainerConfig.Containers.DNSSearches.Get(), "set custom DNS search domains");
	// flags->StringSliceVar(fr->DNSServers, "dns", defaultContainerConfig.Containers.DNSServers.Get(), "set custom DNS servers or disable it completely by setting it to 'none', which prevents the automatic creation of `/etc/resolv.conf`.");
	// flags->StringSliceVar(fr->DNSOptions, "dns-option", defaultContainerConfig.Containers.DNSOptions.Get(), "set custom DNS options");
	flags->BoolVar(fr->HTTPProxy, "http-proxy", true, "pass through HTTP Proxy environment variables");
	// flags->StringVar(fr->Isolation, "isolation", DefaultIsolation(), "`type` of process isolation to use. Use BUILDAH_ISOLATION environment variable to override.");
	flags->StringVar(fr->Memory, "memory","","memory limit (format: <number>[<unit>], where unit = b, k, m or g)");
	flags->StringVar(fr->MemorySwap, "memory-swap", "", "swap limit equal to memory plus swap: '-1' to enable unlimited swap");
	flags->IntVar(fr->Retry, "retry", 3, "number of times to retry in case of failure when performing push/pull");
	// flags->StringVar(fr->RetryDelay, "retry-delay", defaultContainerConfig.Engine.RetryDelay, "delay between retries in case of push/pull failures");
	flags->String("arch", "amd64", "set the ARCH of the image to the provided value instead of the architecture of the host");
	flags->String("os", "Windows10", "prefer `OS` instead of the running OS when pulling images");
	flags->StringSlice("platform", vector<string>{"linux/arm"}, "set the `OS/ARCH[/VARIANT]` of the image to the provided value instead of the current operating system and architecture of the host (for example \"linux/arm\")");
	flags->String("variant", "", "override the `variant` of the specified image");
	flags->StringArrayVar(fr->SecurityOpt, "security-opt", vector<string>(), "security options (default [])");
	// flags->StringVar(fr->ShmSize, "shm-size", defaultContainerConfig.Containers.ShmSize, "size of '/dev/shm'. The format is `<number><unit>`.");
	// flags->StringSliceVar(fr->Ulimit, "ulimit", defaultContainerConfig.Containers.DefaultUlimits.Get(), "ulimit options");
	// flags->StringArrayVar(fr->Volumes, "volume", defaultContainerConfig.Volumes(), "bind mount a volume into the container");

    return flags;
}
Flagset* GetUserNSFlags(UserNSResults* ur){
    Flagset* flags=new Flagset();
	flags->StringSliceVar(ur->GroupAdd, "group-add", vector<string>(), "add additional groups to the primary container process. 'keep-groups' allows container processes to use supplementary groups.");
	flags->StringVar(ur->UserNS, "userns", "", "'container', `path` of user namespace to join, or 'host'");
	flags->StringSliceVar(ur->UserNSUIDMap, "userns-uid-map", vector<string>(), "`containerUID:hostUID:length` UID mapping to use in user namespace");
	flags->StringSliceVar(ur->UserNSGIDMap, "userns-gid-map", vector<string>(), "`containerGID:hostGID:length` GID mapping to use in user namespace");
	flags->StringVar(ur->UserNSUIDMapUser, "userns-uid-map-user", "", "`name` of entries from /etc/subuid to use to set user namespace UID mapping");
	flags->StringVar(ur->UserNSGIDMapGroup, "userns-gid-map-group", "", "`name` of entries from /etc/subgid to use to set user namespace GID mapping");
    return flags;
}
Flagset* GetNameSpaceFlags(NameSpaceResults* nr){
    Flagset* flags=new Flagset();
    flags->StringVar(nr->Cgroup, "cgroupns", "", "'private', or 'host'");
    flags->StringVar(nr->IPC, "ipc", "", "'private', `path` of IPC namespace to join, or 'host'");
	flags->StringVar(nr->Network, "network", "", "'private', 'none', 'ns:path' of network namespace to join, or 'host'");
	flags->StringVar(nr->CNIConfigDir, "cni-config-dir", "", "`directory` of CNI configuration files");
	flags->MarkHidden("cni-config-dir");
	flags->StringVar(nr->CNIPlugInPath, "cni-plugin-path", "", "`path` of CNI network plugins");
	flags->MarkHidden("cni-plugin-path");
	flags->StringVar(nr->PID, "pid", "", "private, `path` of PID namespace to join, or 'host'");
	flags->StringVar(nr->UTS, "uts", "", "private, :`path` of UTS namespace to join, or 'host'");
    return flags;
}
/**
 * @brief 初始化build命令的内容
 * <p>新建一个Command对象用来初始化build命令，完成对build命令的flag的初始化，
 * 并且将初始化好的build对象加入到rootcmd中。
 * 
 */
void init_buildcmd(){
    string build_name="build [context]";
    string build_Short="Build an image using instructions in a Containerfile";
    string build_Long={"Builds an OCI image using instructions in one or more Containerfiles.\n\
                If no arguments are specified, Buildah will use the current working directory\n\
                as the build context and look for a Containerfile. The build fails if no\n\
                Containerfile nor Dockerfile is present."};
    string build_example={"buildah build\n\
                    buildah bud -f Containerfile.simple.\n\
                    buildah bud --volume /home/test:/myvol:ro,Z -t imageName.\n\
                    buildah bud -f Containerfile.simple -f Containerfile.notsosimple."};
    Command* build_Command=new Command(build_name,build_Short,build_Long,build_example);
    //定义使用模板
    string Template=UsageTemplate();
    build_Command->SetUsageTemplate(Template);
    build_Command->Args=MaximumNArgs(1);

    Flagset* flags=build_Command->Flags();
    flags->SetInterspersed(false);
    BuildOptions* br=new BuildOptions();
    // BudResults* buildFlagResults=new BudResults();
    // LayerResults* layerFlagsResults=new LayerResults();
    // FromAndBudResults* fromAndBudResults=new FromAndBudResults();
    // UserNSResults* userNSResults=new UserNSResults();
    // NameSpaceResults* namespaceResults=new NameSpaceResults();
    // BuildOptions* br=new BuildOptions(buildFlagResults,layerFlagsResults,fromAndBudResults,userNSResults);
    
    // br->br=std::make_shared<BudResults>(buildFlagResults);
    // br->br=buildFlagResults;
    Flagset* buildflags=Getbuildflags(br);
    Flagset* layerFlags=GetLayerFlags(br);
    Flagset* fromAndBudFlags=GetFromAndBudFlags(br);
    Flagset* usernsFlags=GetUserNSFlags(br);
    Flagset* namespaceFlags=GetNameSpaceFlags(br);
    flags->AddFlagSet(buildflags);
    flags->AddFlagSet(layerFlags);
    flags->AddFlagSet(fromAndBudFlags);
    flags->AddFlagSet(usernsFlags);
    flags->AddFlagSet(namespaceFlags);
    build_Command->Run=[=](Command& cmd, vector<string> args){
        buildCmd(cmd,args,br);
    };
    rootcmd.AddCommand({build_Command});
    // cout<<"hello buildah-build!"<<endl;
    // build_Command.Run();
    // return build_Command;
}

/**
 * @brief build 命令Run操作的
 * <p>定义build命令的行为
 * @param cmd 用来运行的命令
 * @param args 运行的参数
 */
void buildCmd(Command& cmd, vector<string> args,BuildOptions* iopts){
    try {
        cout<<"hello buildah-build!"<<endl;
        if(cmd.Flag_find("logfile")->changed){
            iopts->logwriter->open(iopts->Logfile,std::ios::out | std::ios::trunc);
            if(!iopts->logwriter){
                throw myerror("Failed to open log file: " + iopts->Logfile);
            }
        }
        define_BuildOptions* budopt=new define_BuildOptions();
        vector<string> ret_containerfiles;
        vector<string> removeAll;
        GenBuildOptions(&cmd,args,iopts,budopt,ret_containerfiles,removeAll);
        auto remove_temp_files = [&removeAll]() {
            for (const auto& f : removeAll) {
                RemoveAll(f);
            }
            return;
        };
        budopt->DefaultMountsFilePath=globalFlagOptions.DefaultMountsFile;
        auto stores=new store();
        try {
            stores = getStore(&cmd);
        } catch (const myerror& e) {
            remove_temp_files();
            throw;
        }

        string id;
        auto ref=new canonical();
        try {
            id= BuildDockerfiles(stores, *budopt, ret_containerfiles,ref);
            if (!budopt->Manifest.empty()) {
                throw myerror("manifest list id = " + id + ", ref = " + ref->String());
                // std::cerr << "manifest list id = " << id << ", ref = " << ref << std::endl;
            }
        } catch (const myerror& e) {
            remove_temp_files();
            throw;
        }

        remove_temp_files();

    } catch (const myerror& e) {
        // std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}
/**
 * @brief 模板定义
 * 
 * @return string 返回模板的字符串
 */
string UsageTemplate(){
    string str{"Usage:{{if .Runnable}}\
                {{.UseLine}}{{end}}{{if .HasAvailableSubCommands}}\
                {{.CommandPath}} [command]{{end}}{{if gt (len .Aliases) 0}}\
                Aliases:\
                {{.NameAndAliases}}{{end}}{{if .HasExample}}\
                Examples:\
                {{.Example}}{{end}}{{if .HasAvailableSubCommands}}\
                Available Commands:{{range .Commands}}{{if (or .IsAvailableCommand (eq .Name \"help\"))}}\
                {{rpad .Name .NamePadding }} {{.Short}}{{end}}{{end}}{{end}}{{if .HasAvailableLocalFlags}}\
                Flags:\
                {{.LocalFlags.FlagUsages | trimTrailingWhitespaces}}{{end}}{{if .HasAvailableInheritedFlags}}\
                {{end}}"};
    return str;
}
// /**
//  * @brief GenBuildOptions 将命令行标志转换为 BuildOptions 结构
//  * 
//  * @param cmd 
//  * @param inputArgs 
//  * @param iopts 
//  * @param budopt 
//  * @param ret_containerfiles 
//  * @param removeAll 
//  */
// void GenBuildOptions(Command* cmd, vector<string> inputArgs,BuildOptions* iopts, define_BuildOptions* budopt, vector<string>& ret_containerfiles,vector<string>& removeAll){

// }
void RemoveAll(const std::string& path) {
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) != 0) {
        return; // 路径不存在
    }

    if (S_ISDIR(path_stat.st_mode)) {
        DIR* dir = opendir(path.c_str());
        if (dir) {
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") {
                    continue;
                }
                std::string fullPath = path + "/" + entry->d_name;
                RemoveAll(fullPath);
            }
            closedir(dir);
        }
        rmdir(path.c_str());
    } else {
        remove(path.c_str());
    }
}