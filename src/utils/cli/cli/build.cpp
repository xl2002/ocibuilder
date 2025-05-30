/**
 * @file build.cpp
 * @brief 实现构建相关的功能函数
 * @details 包含生成构建选项和容器文件处理的核心功能
 * 
 * 主要功能:
 * - GenBuildOptions: 根据命令行参数生成构建选项
 * - getContainerfiles: 处理输入的容器文件路径
 */

#include "utils/cli/cli/build.h"
#include "security/auth.h"
#include "image/util/util.h"
#include "utils/parse/parse.h"
#include "utils/cli/cli/common.h"
#include "image/types/define/types.h"
#include "filesys/systems.h"
#include <cstdio>
#include <iostream>
#include <memory>
#include <chrono>
#include "utils/common/go/file.h"
vector<string> getContainerfiles(vector<string> files);

/**
 * @brief 生成给定命令的构建选项。
 *
 * @param cmd 命令。
 * @param inputArgs 输入参数。
 * @param iopts 构建选项。
 * @param options 指向构建选项的共享指针。
 * @param ret_containerfiles 存储容器文件的向量。
 * @param removeAll 存储要删除的文件的向量。
 *
 *@throws myerror 如果发生错误。
 */
void GenBuildOptions(std::shared_ptr <Command> cmd, vector<string> inputArgs,std::shared_ptr <BuildOptions> iopts, shared_ptr<define_BuildOptions> options, vector<string>& ret_containerfiles,vector<string>& removeAll){
    string output;
    vector<string> tags;
    bool cleanTmpFile=false;
    if(iopts->Network=="none"){
        if(cmd->Flag_find("dns")->changed){
            throw myerror("the --dns option cannot be used with --network=none");
        }
        if(cmd->Flag_find("dns-option")->changed){
            throw myerror("the --dns-option option cannot be used with --network=none");
        }
        if(cmd->Flag_find("dns-search")->changed){
            throw myerror("the --dns-search option cannot be used with --network=none");
        }
    }
    if(cmd->Flag_find("tag")->changed){
        tags=iopts->tag;
        if(tags.size()>0){
            output=tags[0];
            tags.erase(tags.begin());
        }
        if(cmd->Flag_find("manifest")->changed){

        }
    }
    try
    {
        // CheckAuthFile(iopts->Authfile);
        cleanTmpFile=MirrorToTempFileIfPathIsDescriptor(iopts->Authfile,iopts->Authfile);
        if(cleanTmpFile){
            removeAll.emplace_back(iopts->Authfile);
        }
        PullPolicy pullPolicy=PullPolicyFromOptions(cmd);
        map<string,string> args;
        if(cmd->Flag_find("build-arg-file")->changed){}
        if(cmd->Flag_find("build-arg")->changed){}
        auto additionalBuildContext=map<string,shared_ptr<AdditionalBuildContext>>();
        if(cmd->Flag_find("build-context")->changed){}
        ret_containerfiles=getContainerfiles(iopts->File);
        string format=GetFormat(iopts->Format);
        bool layers=UseLayers();
        if(cmd->Flag_find("layers")->changed){
            layers=iopts->Layers;
        }
        string contextDir="";
        vector<string> cliArgs = inputArgs;
        if(cliArgs.size()==0){

        }else{
            string tempDir,subDir;
            TempDirForURL("", "buildah", cliArgs[0],tempDir,subDir);
            if(tempDir!=""){
                removeAll.emplace_back(tempDir);
                contextDir=JoinPath(tempDir,subDir);
            }else{
                // char fullPath[MAX_PATH];
                // if(_fullpath(fullPath,cliArgs[0].c_str(),MAX_PATH)==nullptr){
                //     throw myerror ("Failed to get absolute path");
                // }
                // contextDir=string(fullPath);
                contextDir=Abspath(cliArgs[0]);
            }

        }
        if(ret_containerfiles.size()==0){
            string  containerfile= DiscoverContainerfile(contextDir);
            ret_containerfiles.emplace_back(containerfile);
            contextDir=getDirectory(containerfile);
        }
        contextDir = resolveSymlinks(contextDir);
        // std::shared_ptr<std::istream> stdinStream=nullptr;
        // if (iopts->Stdin) {
        //     stdinStream = std::make_shared<std::istream>(std::cin);
        // }
        if(iopts->logwriter!=nullptr){}
        auto systemContext= SystemContextFromOptions(cmd);
        auto isolation= IsolationOption(iopts->Isolation);
        auto runtimeFlags=vector<string>();
        // for(auto it:iopt)
        auto commonOpts= CommonbuildOptions(cmd);
        if(cmd->Flag_find("rm")->changed || (cmd->Flag_find("force-rm")->changed && (!cmd->Flag_find("layers")->changed && !cmd->Flag_find("no-cache")->changed))){

        }
        if(cmd->Flag_find("compress")->changed){
            throw myerror("--compress option specified but is ignored");
        }
        auto compression=std::make_shared<Compression>();
        compression->value=Gzip;
        if(iopts->DisableCompression){
            compression->value=Uncompressed;
        }
        if(cmd->Flag_find("disable-content-trust")){

        }
        std::shared_ptr<NetworkConfigurationPolicy> networkPolicy=std::make_shared<NetworkConfigurationPolicy>();
        auto namespaceOptions= Namespaceoptions(cmd,networkPolicy);
        auto idmappingoptions=make_shared<IDMappingOptions>();
        auto usernsOption=idmappingOptions(cmd,isolation,idmappingoptions);
        namespaceOptions->AddOrReplace(usernsOption->val);
        auto platforms=PlatformsFromOptions(cmd);
        auto decryptConfig=DecryptConfigs(iopts->DecryptionKeys);
        std::vector<std::string>excludes;
        std::shared_ptr<std::chrono::system_clock::time_point> timestamp;
        if(cmd->Flag_find("timestamp")->changed){

        }
        if(cmd->Flag_find("output")->changed){

        }
        auto confidentialWorkloadOptions=std::make_shared<ConfidentialWorkloadOptions>();
        if(cmd->Flag_find("cw")->changed){

        }
        auto cacheTo=std::vector<named>();
        auto cacheFrom=std::vector<named>();
        if(cmd->Flag_find("cache-to")->changed){

        }
        if(cmd->Flag_find("cache-from")->changed){

        }
        std::chrono::duration<int> cacheTTL(0);
        if(cmd->Flag_find("cache-ttl")->changed){
            
        }
        if(cmd->Flag_find("network")->changed){

        }
        std::vector<SBOMScanOptions> sbomScanOptions;
        // if(cmd->Flag_find("sbom")->changed || cmd->Flag_find("sbom-scanner-command")->changed || cmd->Flag_find("sbom-scanner-image")->changed 
        //     || cmd->Flag_find("sbom-image-output")->changed || cmd->Flag_find("sbom-merge-strategy")->changed || cmd->Flag_find("sbom-output")->changed
        //     || cmd->Flag_find("sbom-purl-output")->changed || cmd->Flag_find("sbom-image-purl-output")->changed){

        // }
        options->check=iopts->check;
        options->AddCapabilities=iopts->CapAdd;
        options->AdditionalBuildContexts=additionalBuildContext;
        options->AdditionalTags=tags;
        options->AllPlatforms=iopts->allplatform;
        options->Annotations=iopts->annotation;
        options->Architecture=systemContext->ArchitectureChoice;
        options->Args=args;
        options->BlobDirectory=iopts->BlobCache;
        options->BuildOutput=iopts->BuildOutput;
        options->CacheFrom=cacheFrom;
        options->CacheTo=cacheTo;
        options->CacheTTL=cacheTTL;
        options->CDIConfigDir=iopts->CDIConfigDir;
        options->CNIConfigDir=iopts->CNIConfigDir;
        options->CNIPluginPath=iopts->CNIPlugInPath;
        options->ConfidentialWorkload=confidentialWorkloadOptions;
        // options->CPPFlags=iopts->CPPFlags;
        options->CommonBuildOpts=commonOpts;
        options->Compression=compression;
        options->ConfigureNetwork=networkPolicy;
        options->ContextDirectory=contextDir;
        options->Devices=iopts->Devices;
        options->DropCapabilities=iopts->CapDrop;
        // options->Err=stderrStream;
        options->Excludes=excludes;
        options->ForceRmIntermediateCtrs=iopts->ForceRm;
        options->From=iopts->From;
        options->GroupAdd=iopts->GroupAdd;
        options->IDMappingoptions=idmappingoptions;
        options->IIDFile=iopts->Iidfile;
        options->IgnoreFile=iopts->IgnoreFile;
        // options->In=stdinStream;
        options->Isolation=isolation;
        options->Jobs=make_shared<int>(iopts->Jobs); // iopts->Jobs;
        options->Labels=iopts->Label;
        options->LayerLabels=iopts->LayerLabel;
        options->Layers=layers;
        options->LogFile=iopts->Logfile;
        options->LogRusage=iopts->LogRusage;
        options->LogSplitByPlatform=iopts->LogSplitByPlatform;
        options->Manifest=iopts->Manifest;
        options->MaxPullPushRetries=iopts->Retry;
        options->Namespaceoptions=namespaceOptions->val;
        options->NoCache=iopts->NoCache;
        options->OS=systemContext->OSChoice;
        options->OSFeatures=iopts->OSFeatures;
        options->OSVersion=iopts->OSVersion;
        options->OciDecryptConfig=decryptConfig;
        // options->Out=stdoutStream;
        options->Output=output;
        options->OutputFormat=format;
        options->PlatformsList=platforms;
        // options->PullPolicy=std::shared_ptr<PullPolicy>(&pullPolicy);
        options->PullPolicy=std::make_shared<PullPolicy>(pullPolicy);
        options->Quiet=iopts->Quiet;
        options->RemoveIntermediateCtrs=iopts->Rm;
        // options->ReportWriter=reporterStream;
        // options->Runtime=iopts->Runtime;
        options->RuntimeArgs=runtimeFlags;
        options->RusageLogFile=iopts->RusageLogFile;
        options->SBOMScanOptions=sbomScanOptions;
        options->SignBy=iopts->SignBy;
        options->SignaturePolicyPath=iopts->SignaturePolicy;
        auto op=NewOptionalBool(iopts->SkipUnusedStages);
        options->SkipUnusedStages=op;
        options->Squash=iopts->Squash;
        options->systemContext=systemContext;
        options->Target=iopts->Target;
        options->Timestamp=timestamp;
        options->TransientMounts=iopts->Volumes;
        options->UnsetEnvs=iopts->UnsetEnvs;
        options->UnsetLabels=iopts->UnsetLabels;
        if(iopts->RetryDelay!=""){

        }
        if(iopts->Quiet){

        }
        options->Envs=LookupEnvVarReferences(iopts->envs);
    }
    catch(const myerror& e)
    {
        throw;
    }
    return;
}
/**
 * @brief 将命令行参数转换为容器文件路径
 * @param files 输入的文件路径列表
 * @details 
 * - 处理特殊路径"-"，将其转换为".//"
 * - 其他路径保持不变
 * @return vector<string> 处理后的容器文件路径列表
 * @note 此函数主要用于规范化输入的容器文件路径
 */
vector<string> getContainerfiles(vector<string> files) {
    vector<string> ret_containerfiles;
    for (auto it : files) {
        if (it == "-") {
            ret_containerfiles.emplace_back(".//");
        } else {
            ret_containerfiles.emplace_back(it);
        }
    }
    return ret_containerfiles;
}
