/**
 * @file images.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 images.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "images.h"

/**
 * @brief 初始化 images 命令的内容
 * 
 */
void init_images(){
    imagesOptions options;
    string name{"images"};
    string Short{"List images in local storage"};
    string Long{"Lists locally stored images."};
    string example{"buildah images --all\n\
                    buildah images [imageName]\n\
                    buildah images --format '{{.ID}} {{.Name}} {{.Size}} {{.CreatedAtRaw}}'"};
    Command* imagesCommand=new Command{name,Short,Long,example};
    // imagesCommand.Run=imagesCmd;
    Flagset* flags=imagesCommand->Flags();
    // flags.BoolVar();

    rootcmd.AddCommand({imagesCommand});
    // return imagesCommand;
}

/**
 * @brief images 命令Run操作的
 * 
 */
void imagesCmd(){

}