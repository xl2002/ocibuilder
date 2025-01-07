/**
 * @file inspect.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 inspect.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "cmd/inspect/inspect.h"
#include "utils/cli/cli/common.h"
#include "image/buildah/buildah.h"
#include <iostream>
#include <vector>
#include "utils/common/json.h"
#include <memory>
#include <boost/json.hpp>
#include <boost/json/src.hpp>
/**
 * @brief 初始化 inspect 命令的内容
 * 
 */
void init_inspect(){
    string name{"inspect"};
    string Short{"Inspect the configuration of a container or image"};
    string Long{"Inspects a build container's or built image's configuration."};
    string example{"buildah inspect imageName/imageID"};
    Command* inspectCommand=new Command(name,Short,Long,example);
    string Template=UsageTemplate();
    inspectCommand->SetUsageTemplate(Template);
    inspectCommand->Run=[=](Command& cmd, vector<string> args){
        inspectCmd(cmd,args);
    };
    rootcmd.AddCommand({inspectCommand});
}

/**
 * @brief inspect返回镜像的信息
 * 
 */
std::shared_ptr<Store> getStore(Command* cmd);
void inspectCmd(Command& cmd, vector<string> args){
    //1. 加载镜像仓库
    std::shared_ptr<Store> store = getStore(&cmd);
    if (!store) {
        std::cerr << "Failed to load image store." << std::endl;
        return;
    }
    //2. 获得指定镜像的信息auto imagestore=store->Image(imageName/imageID);
    if (args.empty()) {
        std::cerr << "No image name or ID provided." << std::endl;
        return;
    }
    string imageNameOrID = args[0];
    std::shared_ptr<storage::Image> imagestore = store->Image(imageNameOrID);
    if (!imagestore) {
        std::cerr << "Image not found: " << imageNameOrID << std::endl;
        return;
    }
    //3. 根据获得imagestore构造BuilderInfo，只需要填充OCI镜像部分
    BuilderInfo builderInfo;
    // 假设 imagestore 中有相应的字段可以填充 builderInfo
    // BuilderInfo builderInfo;
    builderInfo.Type = "OCI";
    builderInfo.FromImage = imagestore->Names.empty() ? "" : imagestore->Names[0];
    builderInfo.FromImageID = imagestore->image_manifest->Config.Digests.String();
    builderInfo.FromImageDigest = imagestore->digest->digest;
    builderInfo.Config = imagestore->Metadata;
    // 使用 Boost.JSON 库将 Manifest 序列化为 JSON 字符串
    builderInfo.Manifest = imagestore->image_manifest;
    // builderInfo.Container = "";  // 假设容器名称为空
    // builderInfo.ContainerID = "";  // 假设容器 ID 为空
    // builderInfo.MountPoint = "";  // 假设挂载点为空
    // builderInfo.ProcessLabel = "";  // 假设进程标签为空
    // builderInfo.MountLabel = "";  // 假设挂载标签为空
    builderInfo.ImageAnnotations = imagestore->image_manifest->Annotations;
    builderInfo.ImageCreatedBy =imagestore->image_config->history[0].createdBy; // 假设创建者为空
    builderInfo.OCIv1 = imagestore->image_config;
    // builderInfo.DefaultMountsFilePath = "";  // 假设默认挂载文件路径为空
    // builderInfo.Isolation = "";  // 假设隔离模式为空
    // builderInfo.NamespaceOptions = std::make_shared<NamespaceOptions>();  // 假设命名空间选项为空
    // builderInfo.Capabilities = {};  // 假设能力列表为空
    // builderInfo.ConfigureNetwork = "";  // 假设网络配置为空
    // builderInfo.CNIPluginPath = "";  // 假设 CNI 插件路径为空
    // builderInfo.CNIConfigDir = "";  // 假设 CNI 配置文件路径为空
    // builderInfo.IDMappingOptions = std::make_shared<IDMappingOptions>();  // 假设用户命名空间映射选项为空
    // builderInfo.History = {};  // 假设历史信息为空
    // builderInfo.Devices = std::make_shared<ContainerDevices>();  // 假设设备信息为空
    // builderInfo.DeviceSpecs = {};  // 假设设备规格为空
    // builderInfo.CDIConfigDir = "";  // 假设 CDI 配置文件路径为空
    //4. 打印镜像信息，unmarshal得到BuilderInfo的json字符串
    boost::json::value jv = boost::json::value_from(builderInfo);
    std::cout<<format_json(jv)<<std::endl;
}