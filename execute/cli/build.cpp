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
        SystemContext*systemContext= SystemContextFromOptions(cmd);
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