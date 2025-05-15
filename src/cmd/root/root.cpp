/**
 * @file root.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 root.h头文件中定义的方法进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "cmd/root/root.h"

// globalFlags globalFlagOptions{};
// int exitcode=0;
// std::shared_ptr<Config> defaultContainerConfig=nullptr;
const string logLevel="log-level";
// Command rootcmd=init_rootcmd(); ///<定义rootcmd，作为全局变量
/**
 * @brief rootcmd的初始化函数
 * 
 * @return Command 返回rootcmd结构
 */
std::shared_ptr <Command> init_rootcmd(){
    auto Appcmd=std::make_shared<Command>();
    Appcmd->name="ocibuilder";
    Appcmd->Long="A tool that facilitates building OCI images";
    Appcmd->version="image-spec 1.0.0";
    Appcmd->Run=[](std::shared_ptr<Command> cmd, vector<string> args){cmd->Help();};
    Appcmd->PersistentPreRun=[](std::shared_ptr<Command> cmd, vector<string> args){before(cmd);};
    Appcmd->PersistentPostRun=[](std::shared_ptr<Command> cmd, vector<string> args){after(cmd);};
    ///初始化运行环境
    StoreOptions storageOptions;
    try{
        storageOptions=DefaultStoreOptions();
    }catch(const myerror& e){
        e.logerror();
        exit(1);
    }
    vector<string> defaultStoreDriverOptions;
    if(storageOptions.graph_driver_options.size()>0){
        defaultStoreDriverOptions= storageOptions.graph_driver_options;
    }
    try{
        defaultContainerConfig=Config_defaut();
    }catch(const myerror& e){
        e.logerror();
        exit(1);
    }
    defaultContainerConfig->CheckCgroupsAndAdjustConfig();

    auto persistentflags=Appcmd->PersistentFlags();
    persistentflags->StringVar(globalFlagOptions.Root,"root",storageOptions.graph_root,"storage root dir");
    persistentflags->StringVar(globalFlagOptions.RegistriesConf,"registries-conf","","path to registries.conf file (not usually used)");
    persistentflags->StringVar(globalFlagOptions.RegistriesConfDir,"registries-conf-dir", "", "path to registries.conf.d directory (not usually used)");
    persistentflags->BoolVar(globalFlagOptions.Debug,"debug",false, "print debugging information");
    persistentflags->StringVar(globalFlagOptions.DefaultMountsFile, "default-mounts-file", "", "path to default mounts file");
    persistentflags->StringVar(globalFlagOptions.UserShortNameAliasConfPath, "short-name-alias-conf", "", "path to short name alias cache file (not usually used)");
    try{
        persistentflags->MarkHidden("debug");///<用help时，mark掉的flag不显示
        persistentflags->MarkHidden("short-name-alias-conf");
        persistentflags->MarkHidden("default-mounts-file");
        persistentflags->MarkHidden("registries-conf");
        persistentflags->MarkHidden("registries-conf-dir");
        persistentflags->MarkHidden("root");
    }catch(const myerror& e){
        cerr<<"unable to mark cpu-profile flag as hidden: "<<e.what()<<endl;
        // exit(1);
    }
    
    
    return Appcmd;
}




void before(std::shared_ptr<Command> cmd){

}
void after(std::shared_ptr<Command> cmd){

}