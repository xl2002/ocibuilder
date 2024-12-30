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
#include "cmd/images/images.h"
#include "utils/cli/cli/common.h"
#include <boost/format.hpp>
#include "storage/storage/storage.h"
/**
 * @brief 初始化 images 命令的内容
 * 
 */
void init_images(){
    std::shared_ptr<imagesOptions> options=std::make_shared<imagesOptions>();
    string name{"images"};
    string Short{"List images in local storage"};
    string Long{"Lists locally stored images."};
    string example{"buildah images --all\n  buildah images [imageName]\n  buildah images --format '{{.ID}} {{.Name}} {{.Size}} {{.CreatedAtRaw}}'"};
    Command* imagesCommand=new Command{name,Short,Long,example};
    string Template=UsageTemplate();
    imagesCommand->SetUsageTemplate(Template);
    Flagset* flags=imagesCommand->Flags();
    flags->BoolVar(options->all,"all",false,"show all images, including intermediate images from a build");
    imagesCommand->Run=[=](Command& cmd, vector<string> args){
        imagesCmd(cmd,args,options);
    };
    rootcmd.AddCommand({imagesCommand});
    // return imagesCommand;
}
/**
 * @brief 格式化打印所有镜像信息
 * 
 * @param images 
 */
void formatImages(std::vector<storage::Image> images){
    //格式化打印所有镜像信息,采用boost库的format函数
    
}
/**
 * @brief images 命令Run操作的
 * 
 */
void imagesCmd(Command& cmd, vector<string> args,std::shared_ptr<imagesOptions> iopts){
    //1. 加载本地镜像库

    //2. 获得所有镜像信息
    //std::vector<storage::Image> ImageStore::Images();

    //3. 格式化打印所有镜像信息
}