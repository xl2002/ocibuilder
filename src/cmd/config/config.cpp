/**
 * @file config.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 config.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "cmd/config/config.h"
#include "utils/cli/cli/common.h"
#include "image/buildah/buildah.h"
#include "storage/storage/storage.h"
// #include "command.h"

/**
 * @brief 初始化 config 命令的内容
 *
 * 这个函数用于初始化 config 命令的内容，它创建了一个 config 命令对象，并将其添加到根命令中。
 * 该命令对象具有以下属性：
 * - 命令名称为 "config"
 * - 简短介绍为 "Update image configuration settings"
 * - 详细介绍为 "Modifies the configuration values which will be saved to the image."
 * - 使用示例如下：
 *   - buildah config --author='Jane Austen' --workingdir='/etc/mycontainers' containerID
 *   - buildah config --entrypoint '[ \"/entrypoint.sh\", \"dev\" ]' containerID
 *   - buildah config --env foo=bar --env PATH=$PATH containerID
 *
 * 函数内部不执行 config 命令的 Run 操作，因此需要在其他地方定义 configCmd 函数。
 *
 * @return void
 */
void init_configcmd(){
    std::shared_ptr<configOptions> options=std::make_shared<configOptions>();
    string name{"config"};
    string Short{"Update image configuration settings"};
    string Long{"Modifies the configuration values which will be saved to the image."};
    string example{"buildah config --author='Jane Austen' --workingdir='/etc/mycontainers' containerID\n  buildah config --entrypoint '[ \"/entrypoint.sh\", \"dev\" ]' containerID\n  buildah config --env foo=bar --env PATH=$PATH containerID"};
    
    Command* configCommand=new Command(name,Short,Long,example);
    string Template=UsageTemplate();
    configCommand->SetUsageTemplate(Template);
    Flagset* flags=configCommand->Flags();
    flags->SetInterspersed(false);
    //初始化每个flag的内容
    flags->BoolVar(options->addHistory,"add-history",false,"add an entry for this operation to the image's history.  Use BUILDAH_HISTORY environment variable to override. (default false)");
    flags->StringArrayVar(options->annotation,"annotation",vector<string>(),"add `annotation` e.g. annotation=value, for the target image (default [])");
    flags->StringVar(options->arch,"arch","","set the architecture of the image (e.g., amd64, arm, ppc64le) (default unset)");
    flags->StringVar(options->author,"author","","set the author of the image (e.g., Jane Austen) (default unset)");
    flags->StringVar(options->cmd,"cmd","","set the default `command` to run for containers based on the image");
    flags->StringArrayVar(options->env,"env",vector<string>(), "set an environment variable for the target image (e.g., foo=bar) (default [])");
    flags->StringVar(options->entrypoint,"entrypoint","", "set the entrypoint for the target image (default [])");
    flags->StringArrayVar(options->label,"label",vector<string>(), "set a label for the target image (e.g., label=value) (default [])");
    flags->StringVar(options->os,"os","","set the OS of the image (e.g., linux, windows) (default unset)");
    
    configCommand->Run=[=](Command& cmd, vector<string> args){
        configCmd(cmd,args,options);
    };
    rootcmd.AddCommand({configCommand});
    // return configCommand;
}
/**
 * @brief 加载镜像库中的镜像，构建新的Builder对象
 * 
 * @param store 
 * @param name 
 * @return std::shared_ptr<Builder> 
 */
std::shared_ptr<Builder> openBuilder(std::shared_ptr<Store> store,std::string name){
    //imagestore中存储了镜像的信息，包括config，manifest等
    auto imagestore=store->Image(name);

    //构建新的Builder对象，根据imagestore中的config和manifest，构建新的Builder对象

    return nullptr;
}
/**
 * @brief 更新Builder对象中的镜像配置
 * 
 * @param builder 
 * @param c 
 * @param iopts 
 */
void updateConfig(std::shared_ptr<Builder> builder,Command& c,std::shared_ptr<configOptions> iopts){
    //根据iops中的配置，更新builder中的镜像配置
    //只关注我们实现的部分

}
/**
 * @brief config命令Run操作的
 * 
 */
void configCmd(Command& cmd, vector<string> args,std::shared_ptr<configOptions> iopts){
    //1. 加载存储store

    //2. 构建镜像的builder

    //3. 更新builder中镜像的配置

    //4. 保存镜像配置
    return;
}