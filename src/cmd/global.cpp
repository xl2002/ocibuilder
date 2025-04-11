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
 * @brief 初始化命令模块的全局配置
 * @details 此函数完成以下初始化：
 * - 设置环境变量替换的白名单
 * - 配置 Dockerfile 指令的分发表
 * - 初始化平台规范参数 (TARGETPLATFORM/BUILDPLATFORM 等)
 * - 注册内置构建参数白名单 (NO_PROXY/no_proxy)
 * - 创建默认容器运行时配置
 * - 初始化 ONBUILD 指令正则匹配器
 * 
 * @note 此函数需要在所有命令执行前调用，全局只需初始化一次
 * 
 * 初始化内容包括：
 * 1. replaceEnvAllowed 映射表 (env, volume, workdir 等指令的变量替换权限)
 * 2. allowWordExpansion 映射表 (EXPOSE 指令参数扩展权限的特殊处理)
 * 3. 构建参数白名单 builtinAllowedBuildArgs
 * 4. Dockerfile 分发表 evaluateTable
 * 5. 平台规范参数 builtinBuildArgs
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
        {dockerfilecommand::Volume,Volume},
        {dockerfilecommand::Workdir,workdir}
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