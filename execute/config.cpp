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
#include "cmd/config.h"
// #include "command.h"

/**
 * @brief 初始化 config 命令的内容
 * 
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