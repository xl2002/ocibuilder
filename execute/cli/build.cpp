#include "cli/build.h"
#include "pkg/auth/auth.h"
#include "pkg/util/util.h"
#include "pkg/parse/parse.h"
#include "cli/common.h"
#include "define/types.h"
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <chrono>
vector<string> getContainerfiles(vector<string> files);

void GenBuildOptions(Command* cmd, vector<string> inputArgs,BuildOptions* iopts, define_BuildOptions* options, vector<string>& ret_containerfiles,vector<string>& removeAll){
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
        CheckAuthFile(iopts->Authfile);
        cleanTmpFile=MirrorToTempFileIfPathIsDescriptor(iopts->Authfile,iopts->Authfile);
        if(cleanTmpFile){
            removeAll.emplace_back(iopts->Authfile);
        }
        PullPolicy pullPolicy=PullPolicyFromOptions(cmd);
        map<string,string> args;
        if(cmd->Flag_find("build-arg-file")->changed){}
        if(cmd->Flag_find("build-arg")->changed){}
        if(cmd->Flag_find("build-context")->changed){}
        vector<string> containerfiles=getContainerfiles(iopts->File);
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
                char fullPath[MAX_PATH];
                if(_fullpath(fullPath,cliArgs[0].c_str(),MAX_PATH)!=nullptr){
                    throw myerror ("Failed to get absolute path");
                }
                contextDir=string(fullPath);
            }

        }
        if(containerfiles.size()==0){
            string  containerfile= DiscoverContainerfile(contextDir);
            containerfiles.emplace_back(containerfile);
            contextDir=getDirectory(containerfile);
        }
        contextDir = resolveSymlinks(contextDir);
        std::istream* stdinStream=nullptr;
        if (iopts->Stdin) {
            stdinStream = &std::cin;
        }
        std::ostream* stdoutStream = &std::cout;
        std::ostream* stderrStream = &std::cerr;
        std::ostream* reporterStream = &std::cerr;
        if(iopts->logwriter!=nullptr){}
        auto systemContext= SystemContextFromOptions(cmd);
        auto isolation= IsolationOption(iopts->Isolation);
        // auto runtimeFlags=vector<string>();
        // for(auto it:iopt)
        auto commonOpts= CommonbuildOptions(cmd);
        if(cmd->Flag_find("rm")->changed || cmd->Flag_find("force-rm")->changed && (!cmd->Flag_find("layers")->changed && !cmd->Flag_find("no-cache")->changed)){

        }
        if(cmd->Flag_find("compress")->changed){
            throw myerror("--compress option specified but is ignored");
        }
        auto compression=Gzip;
        if(iopts->DisableCompression){
            compression=Uncompressed;
        }
        if(cmd->Flag_find("disable-content-trust")){

        }
        NetworkConfigurationPolicy networkPolicy;
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
        auto confidentialWorkloadOptions=ConfidentialWorkloadOptions();
        if(cmd->Flag_find("cw")->changed){

        }
        auto cacheTo=std::vector<Named>();
        auto cacheFrom=std::vector<Named>();
        if(cmd->Flag_find("cache-to")->changed){

        }
        if(cmd->Flag_find("cache-from")->changed){

        }
        std::chrono::duration<int> cacheTTL();
        if(cmd->Flag_find("cache-ttl")->changed){
            
        }
        if(cmd->Flag_find("network")->changed){

        }
        std::vector<SBOMScanOptions> sbomScanOptions;
        // if(cmd->Flag_find("sbom")->changed || cmd->Flag_find("sbom-scanner-command")->changed || cmd->Flag_find("sbom-scanner-image")->changed 
        //     || cmd->Flag_find("sbom-image-output")->changed || cmd->Flag_find("sbom-merge-strategy")->changed || cmd->Flag_find("sbom-output")->changed
        //     || cmd->Flag_find("sbom-purl-output")->changed || cmd->Flag_find("sbom-image-purl-output")->changed){

        // }


        if(iopts->RetryDelay!=""){}
        if(iopts->Quiet){

        }
    }
    catch(const myerror& e)
    {
        throw;
    }
    
}
vector<string> getContainerfiles(vector<string> files){
    vector<string>containerfiles;
    for(auto it:files){
        if(it=="-"){
            containerfiles.emplace_back("D://");
        }else{
            containerfiles.emplace_back(it);
        }
    }
    return containerfiles;
}