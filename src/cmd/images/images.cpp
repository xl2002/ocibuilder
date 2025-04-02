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
    imagesOptions* options=new imagesOptions();
    string name{"images"};
    string Short{"List images in local storage"};
    string Long{"Lists locally stored images."};
    string example{"ocibuilder images --all\n  ocibuilder images [imageName]\n  ocibuilder images --format '{{.ID}} {{.Name}} {{.Size}} {{.CreatedAtRaw}}'"};
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
std::string formatSize(int64_t sizeInBytes) {
    if (sizeInBytes < 1024) {
        // 小于 1 KB
        return boost::str(boost::format("%d B") % sizeInBytes);
    } else if (sizeInBytes < 1024 * 1024) {
        // 小于 1 MB
        double size = sizeInBytes / 1024.0;
        return boost::str(boost::format("%.1f KB") % size);
    } else if (sizeInBytes < 1024 * 1024 * 1024) {
        // 小于 1 GB
        double size = sizeInBytes / (1024.0 * 1024.0);
        return boost::str(boost::format("%.1f MB") % size);
    } else {
        // 大于等于 1 GB
        double size = sizeInBytes / (1024.0 * 1024.0 * 1024.0);
        return boost::str(boost::format("%.1f GB") % size);
    }
}

std::string formatTime(const std::chrono::system_clock::time_point& created) {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto duration = duration_cast<seconds>(now - created);

    // 如果小于60秒，显示秒数
    if (duration < seconds(60)) {
        return boost::str(boost::format("%d seconds ago") % duration.count());
    }

    // 如果小于60分钟，显示分钟数
    auto duration_minutes = duration_cast<minutes>(now - created);
    if (duration_minutes < minutes(60)) {
        return boost::str(boost::format("%d minutes ago") % duration_minutes.count());
    }

    // 如果小于24小时，显示小时数
    auto duration_hours = duration_cast<hours>(now - created);
    if (duration_hours < hours(24)) {
        return boost::str(boost::format("%d hours ago") % duration_hours.count());
    }

    // 如果超过24小时，显示天数
    auto duration_days = duration_cast<seconds>(now - created).count() / (24 * 60 * 60);
    return boost::str(boost::format("%d days ago") % duration_days);
}
/**
 * @brief 格式化打印所有镜像信息
 * 
 * @param images 
 */
void formatImages(const std::vector<storage::Image>& images) {
    // 打印表头
    std::cout << boost::format("%-40s %-10s %-15s %-20s %s\n") % "REPOSITORY" % "TAG" % "IMAGE ID" % "CREATED" % "SIZE";

    // 遍历每个镜像
    for (const auto& image : images) {
        // 提取 repository 和 tag
        // std::string repository = image.Names.empty() ? "none" : image.Names[1].substr(0, image.Names[1].find(':'));
        size_t pos = image.Names[0].rfind(':');
        std::string repository = image.Names[0].substr(0, pos);
        std::string tag = image.Names.empty() || image.Names[0].find(':') == std::string::npos ? "latest" : image.Names[0].substr(pos + 1);

        std::string imageID = image.ID;
        std::string created = formatTime(image.image_config->created);
        std::string size = formatSize(
            std::accumulate(
                image.image_manifest->Layers.begin(),
                image.image_manifest->Layers.end(),
                static_cast<size_t>(0),  // 初始值为 0
                [](size_t total, const Descriptor& layer) {
                    return total + layer.Size;
                }
            )
        );
        // 格式化并打印镜像信息
        std::cout << boost::format("%-40s %-10s %-15s %-20s %s\n") % repository % tag % imageID % created % size;
    }
}
/**
 * @brief images 命令Run操作的
 * 
 */
void imagesCmd(Command& cmd, vector<string> args,imagesOptions*iopts){
    //1. 加载本地镜像库
    auto store = getStore(&cmd);
    auto imagestore = store->image_store;

    //2. 获得所有镜像信息
    std::vector<storage::Image> images;
    if(args.empty()){
        images = imagestore->Images();
    }else{
        images.push_back(*store->Image(args[0]));
    }
    //3. 格式化打印所有镜像信息
    formatImages(images);
    delete iopts;
}