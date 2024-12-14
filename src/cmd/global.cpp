#include "utils/init/init_global.h"
#include "cmd/build/imagebuilder/evaluator.h"
#include "cmd/root/root.h"
#include "cmd/build/imagebuilder/builder.h"
#include "cmd/build/imagebuilder/dispatchers.h"

// 全局变量定义
std::map<std::string,bool> replaceEnvAllowed;
std::map<std::string,bool> allowWordExpansion;

                        ///<声明全局根命令
std::shared_ptr<Config> defaultContainerConfig;    ///<声明全局镜像config
int exitcode;                            ///<声明全局程序结束标志
globalFlags globalFlagOptions;           ///<声明全局标签选项

std::map<std::string,bool> builtinAllowedBuildArgs;
std::map<std::string,StepFunc> evaluateTable;

std::shared_ptr<Regexp> obRgex;
std::shared_ptr<Platform> localspec;
std::map<std::string, std::string>builtinBuildArgs;
/**
 * @brief 初始化cmd模块的全局变量
 * 
 */
void init_cmd(){
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
    
    defaultContainerConfig=nullptr;
    globalFlagOptions={};
    exitcode=0;
    builtinAllowedBuildArgs={
        {"NO_PROXY",true},
        {"no_proxy",true}
    };
    evaluateTable={
        {dockerfilecommand::Env,env},
        {dockerfilecommand::From,from},
        {dockerfilecommand::Label,label},
        {dockerfilecommand::Copy,dispatchCopy},
        {dockerfilecommand::Expose,expose},
        {dockerfilecommand::Entrypoint,entrypoint},
        {dockerfilecommand::Volume,Volume}
    };
    obRgex=Delayed(R"(^\s*ONBUILD\s*)");
    localspec = DefaultSpec();
    builtinBuildArgs = {
        {"TARGETPLATFORM", localspec->OS + "/" + localspec->Architecture},
        {"TARGETOS", localspec->OS},
        {"TARGETARCH", localspec->Architecture},
        {"TARGETVARIANT", localspec->Variant},
        {"BUILDPLATFORM", localspec->OS + "/" + localspec->Architecture},
        {"BUILDOS", localspec->OS},
        {"BUILDARCH", localspec->Architecture},
        {"BUILDVARIANT", localspec->Variant}
    };

}