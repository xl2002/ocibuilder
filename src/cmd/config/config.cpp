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
void init_config(){
    configOptions options;
    string name{"config"};
    string Short{"Update image configuration settings"};
    string Long{"Modifies the configuration values which will be saved to the image."};
    string example{"buildah config --author='Jane Austen' --workingdir='/etc/mycontainers' containerID\n\
                    buildah config --entrypoint '[ \"/entrypoint.sh\", \"dev\" ]' containerID\n\
                    buildah config --env foo=bar --env PATH=$PATH containerID"};
    
    Command* configCommand=new Command(name,Short,Long,example);
    // configCommand.Run=configCmd;
    Flagset* flags=configCommand->Flags();
    //初始化每个flag的内容
    // flags.StringVar();

    rootcmd.AddCommand({configCommand});
    // return configCommand;
}
/**
 * @brief config命令Run操作的
 * 
 */
void configCmd(){

}