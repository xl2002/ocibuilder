/**
 * @file pull.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 pull.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "main/pull.h"
/**
 * @brief 初始化 pull 命令的内容
 * 
 */
void init_pull(){
    pullOptions options;
    string name{"pull"};
    string Short{"Pull an image from the specified location"};
    string Long{"Pulls an image from a registry and stores it locally.\n\
                An image can be pulled using its tag or digest. If a tag is not\n\
                specified, the image with the 'latest' tag (if it exists) is pulled."};
    string example{"buildah pull imagename\n\
                    buildah pull docker-daemon:imagename:imagetag\n\
                    buildah pull myregistry/myrepository/imagename:imagetag"};
    Command* pullCommand=new Command{name,Short,Long,example};
    // pullCommand.Run=pullCmd;
    Flagset* flags=pullCommand->Flags();
    // flags.BoolVar();

    rootcmd.AddCommand({pullCommand});
    // return imagesCommand;
}

/**
 * @brief pull 命令Run操作的
 * 
 */
void pullCmd(){

}
