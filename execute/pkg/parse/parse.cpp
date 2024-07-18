#include "pkg/parse/parse.h"
#include <algorithm>

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

SystemContext* SystemContextFromOptions(Command* c){
    Flagset* flags=c->Flags();
    // auto findFlagFunc=Command::Flag_find;
    try
    {
        string certDir=flags->GetString("cert-dir");
        SystemContext* ctx=new SystemContext();
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
        // ctx->BigFilesTemporaryDir=
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return nullptr;
}