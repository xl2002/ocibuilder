/**
 * @file save.cpp
 * @author zhaoqinlin
 * @brief 对 save.h头文件中的声明进行定义 修改自push.cpp
 * @version 0.1
 * @date 2025/3/3
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "cmd/save/save.h"
#include "network/network.h"
#include "utils/cli/cli/common.h"
#include "security/auth.h"
#include "utils/common/go/string.h"
#include "utils/common/go/file.h"
#include "image/digest/digest.h"
#include "cmd/login/login.h"
#include "image/buildah/tar.h"
//namespace json = boost::json;

/**
 * @brief 初始化 push 命令的内容
 *
 */
void init_save()
{
    auto opts = new saveOptions();
    string name{"save"};
    string Short{"Save an image to a specified destination"};
    string Long{"Saves an image to a specified location."};
    string example{"buildah save image1:latest --output D:/test/image1:imagetest:latest"};
    Command *saveCommand = new Command{name, Short, Long, example};
    string Template = UsageTemplate();
    saveCommand->SetUsageTemplate(Template);

    Flagset *flags = saveCommand->Flags();
    // flags.StringVar();
    flags->SetInterspersed(false);//TODO: 对三条参数的解析
    //flags->StringVar(opts->format, "format", std::string(), "manifest type (oci, v2s1, or v2s2) to use in the destination (default is manifest type of source, with fallbacks)");
    //flags->StringVar(opts->authfile, "authfile", std::string(), "path of the authentication file. Use REGISTRY_AUTH_FILE environment variable to override");
    flags->StringVar(opts->output, "output", std::string(), "path to save the image tar file (required)");
    saveCommand->Run = [=](Command &cmd, vector<string> args)
    {
         // 检查必选参数
         if (args.size()< 3) {
            std::cerr << "the number of arguments is not right!!" << std::endl;
            return;
        }
        // 解析镜像名称
        opts->image = args[0];
        opts->output = args[2];
        // 检查 --output 是否提供
        if (opts->output.empty()) {
            std::cerr << "Error: The --output flag is required." << std::endl;
            return;
        }
        // 调用保存镜像的逻辑
        //std::cout<<opts->output << std::endl;
        saveCmd(cmd, args, opts);

    };
    rootcmd.AddCommand({saveCommand});
    // return imagesCommand;
}

std::string extractPath(const std::string &param)
{

    // 使用 boost::filesystem 解析路径
    boost::filesystem::path filePath(param);

    // 提取父目录
    boost::filesystem::path parentPath = filePath.parent_path();

    // 转换为绝对路径
    boost::filesystem::path absolutePath = boost::filesystem::absolute(parentPath);
    
    return absolutePath.string();
}
/**
 * 将 OCI 配置文件转换为 Docker 配置文件格式
 *
 * @param ociConfigPath   OCI 配置文件的路径
 * @param dockerConfigPath 目标 Docker 配置文件的路径
 * @param configDigest    配置文件的摘要（用于生成 Docker 配置文件的 id 字段）
 * @return bool           成功返回 true，失败返回 false
 */
bool convertOciConfigToDockerLayerConfig(const std::string &ociConfigPath, const std::string &dockerConfigPath, const std::string &configDigest) {
    try {
        // 读取 OCI 配置文件内容
        fs::ifstream configFile(ociConfigPath, std::ios::binary);
        if (!configFile.is_open()) {
            std::cerr << "Failed to open OCI config file: " << ociConfigPath << std::endl;
            return false;
        }
        std::string configContent((std::istreambuf_iterator<char>(configFile)), std::istreambuf_iterator<char>());
        configFile.close();

        // 解析 OCI 配置文件
        json::value ociConfig = json::parse(configContent);

        // 创建 Docker 格式的配置文件
        json::value dockerConfig(json::object_kind); // 创建一个 JSON 对象

        // 填充 Docker 配置文件的基本字段
        dockerConfig.as_object()["id"] = configDigest; // 使用配置文件的摘要作为 ID
        dockerConfig.as_object()["created"] = ociConfig.at("created").as_string(); // 直接复制创建时间
        dockerConfig.as_object()["architecture"] = ociConfig.at("architecture").as_string(); // 直接复制架构
        dockerConfig.as_object()["os"] = ociConfig.at("os").as_string(); // 直接复制操作系统

        // 填充 container_config 字段
        json::value containerConfig(json::object_kind); // 创建一个 JSON 对象
        containerConfig.as_object()["Hostname"] = "";
        containerConfig.as_object()["Domainname"] = "";
        containerConfig.as_object()["User"] = "";
        containerConfig.as_object()["AttachStdin"] = false;
        containerConfig.as_object()["AttachStdout"] = false;
        containerConfig.as_object()["AttachStderr"] = false;
        containerConfig.as_object()["Tty"] = false;
        containerConfig.as_object()["OpenStdin"] = false;
        containerConfig.as_object()["StdinOnce"] = false;
        containerConfig.as_object()["Env"] = json::array(); // 初始化为空数组
        containerConfig.as_object()["Cmd"] = json::array(); // 初始化为空数组
        containerConfig.as_object()["Image"] = "";
        containerConfig.as_object()["Volumes"] = json::object(); // 初始化为空对象
        containerConfig.as_object()["WorkingDir"] = "";
        containerConfig.as_object()["Entrypoint"] = json::array(); // 初始化为空数组
        containerConfig.as_object()["OnBuild"] = json::array(); // 初始化为空数组
        containerConfig.as_object()["Labels"] = json::object(); // 初始化为空对象
        dockerConfig.as_object()["container_config"] = containerConfig;

        // 填充 config 字段
        json::value config(json::object_kind); // 创建一个 JSON 对象
        config.as_object()["Hostname"] = "";
        config.as_object()["Domainname"] = "";
        config.as_object()["User"] = "";
        config.as_object()["AttachStdin"] = false;
        config.as_object()["AttachStdout"] = false;
        config.as_object()["AttachStderr"] = false;
        config.as_object()["Tty"] = false;
        config.as_object()["OpenStdin"] = false;
        config.as_object()["StdinOnce"] = false;

        // 从 OCI 配置中提取 Env 字段
        if (ociConfig.at("config").as_object().contains("Env")) {
            config.as_object()["Env"] = ociConfig.at("config").at("Env");
        } else {
            config.as_object()["Env"] = json::array(); // 如果不存在，初始化为空数组
        }

        // 从 OCI 配置中提取 Cmd 字段
        if (ociConfig.at("config").as_object().contains("Cmd")) {
            config.as_object()["Cmd"] = ociConfig.at("config").at("Cmd");
        } else {
            config.as_object()["Cmd"] = json::array(); // 如果不存在，初始化为空数组
        }

        config.as_object()["Image"] = "";
        config.as_object()["Volumes"] = json::object(); // 初始化为空对象
        config.as_object()["WorkingDir"] = ociConfig.at("config").at("WorkingDir").as_string(); // 直接复制工作目录
        config.as_object()["Entrypoint"] = json::array(); // 初始化为空数组
        config.as_object()["OnBuild"] = json::array(); // 初始化为空数组
        config.as_object()["Labels"] = json::object(); // 初始化为空对象
        dockerConfig.as_object()["config"] = config;

        // 将 Docker 配置文件写入目标路径
        fs::ofstream dockerConfigFile(dockerConfigPath, std::ios::binary);
        if (!dockerConfigFile.is_open()) {
            std::cerr << "Failed to open Docker config file for writing: " << dockerConfigPath << std::endl;
            return false;
        }
        dockerConfigFile << marshal(dockerConfig);
        dockerConfigFile.close();

        return true;
    } catch (const std::exception &e) {
        std::cerr << "Error converting OCI config to Docker config: " << e.what() << std::endl;
        return false;
    }
}


void saveCmd(Command &cmd, vector<string> args, saveOptions * iopts)
{
    std::string destPath, withinTransport, destSpec, newImageName;
    
    // 1. 获得令牌.save不需要
    //CheckAuthFile(iopts);
    // 镜像名
    withinTransport = args[0];
    destPath = iopts->output;//args[1];

    // 读取本地镜像的数据
    auto store = getStore(&cmd);
    // 获得index.json
    std::string indexPath = store.get()->image_store_dir + "/index.json";

    // 得到或创建目标路径
    destSpec = extractPath(destPath)+"/tmp";
    //size_t lastColon = destPath.rfind(':');
    //size_t secondLastColon = destPath.rfind(':', lastColon - 1);
    size_t lastColon = destPath.rfind('/');
    newImageName = destPath.substr(lastColon +1);
    // destSpec = "C:\\Users\\admin\\Documents\\output";
    // std::tie(std::ignore, destPath, std::ignore) = Cut(destPath, ':');
    destPath = destSpec ;//+ "/blobs/sha256";
    if (!fs::exists(destPath))
    {
        fs::create_directories(destPath);
    }

    // 1. 读取index.json
    boost::filesystem::ifstream indexfile(indexPath, std::ios::binary);
    std::ostringstream indexBuffer;
    indexBuffer << indexfile.rdbuf();
    std::string indexContent = indexBuffer.str();
    auto index = unmarshal<storage::index>(indexContent);
    // 从index.json中找到manifest
    int manifest_index = -1;
    for (int i = 0; i < index.manifests.size(); i++)
    {
        // 是否需要直接把命令行中的镜像名替换为localhost/镜像名?
        if (index.manifests[i].annotations["org.opencontainers.image.ref.name"] == "localhost/" + withinTransport)
        {
            manifest_index = i;
            break;
        }
    }
    if (manifest_index == -1)
    {
        std::cerr << "Invalid imageName" << std::endl;
        return;
    }

    // 2. 找到manifest文件
    std::string manifestPath = store.get()->image_store_dir + "/blobs/sha256/" + index.manifests[manifest_index].digest.substr(7);
    if (!fs::exists(manifestPath))
    {
        std::cerr << "Manifest file not found" << std::endl;
        return;
    }

    boost::filesystem::ifstream manifestfile(manifestPath, std::ios::binary);
    std::ostringstream manifestBuffer;
    manifestBuffer << manifestfile.rdbuf();
    std::string manifestContent = manifestBuffer.str();
    auto manifest = unmarshal<::Manifest>(manifestContent);

    // 3. 找到config文件
    std::string configPath = store.get()->image_store_dir + "/blobs/sha256/" + manifest.Config.Digests.digest.substr(7);
    if (!fs::exists(configPath))
    {
        std::cerr << "Config file not found" << std::endl;
        return;
    }
    // 复制配置文件并重命名为 <sha256>.json
    std::string configDest = destSpec + "/"+ manifest.Config.Digests.digest.substr(7)+".json";   //+  "config.json";//+ manifest.Config.Digests.digest.substr(7)
    fs::copy_file(configPath, configDest, fs::copy_options::overwrite_existing);
    //convertOciConfigToDockerConfig(configPath, configDest,manifest.Config.Digests.digest.substr(7));
    //TODO config顺序有变
    // 4. 找到blobs文件
    // std::vector<std::string> blobPath;
    // json::array layers;
    // for (int i = 0; i < manifest.Layers.size(); i++)
    // {
    //     std::string tmp = store.get()->image_store_dir + "/blobs/sha256/" + manifest.Layers[i].Digests.digest.substr(7);
    //     if (!fs::exists(tmp))
    //     {
    //         std::cout << "Blob does not exist: " << std::endl;
    //         return;
    //     }
    //     blobPath.push_back(tmp);
    //     layers.push_back(tmp.substr(tmp.find_last_of('/') + 1));
    // }
    std::vector<std::string> layerPaths;
    std::vector<std::string> layers;//json::array
    
    // for (auto &layer : manifest.at("layers").as_array()) {
    //     std::string layerPath = store.get()->image_store_dir + "/blobs/sha256/" + layer.at("digest").as_string().substr(7);
    //     if (!fs::exists(layerPath)) {
    //         std::cerr << "Layer not found: " << layerPath << std::endl;
    //         return;
    //     }
    //     layerPaths.push_back(layerPath);
    //     layers.push_back(layerPath.substr(layerPath.find_last_of('/') + 1));
    // }
    for (auto &layer : manifest.Layers) {
        std::string layerPath = store.get()->image_store_dir + "/blobs/sha256/" + layer.Digests.digest.substr(7);
        if (!fs::exists(layerPath)) {
            std::cerr << "Layer not found: " << layerPath << std::endl;
            return;
        }
        layerPaths.push_back(layerPath);
        layers.push_back(layerPath.substr(layerPath.find_last_of('/') + 1)+"/layer.tar");
    }
    json::array dockerManifest;
    json::object dockerEntry;
    dockerEntry["Config"] = manifest.Config.Digests.digest.substr(7)+".json";
    dockerEntry["RepoTags"] = { newImageName + ":latest" };
    dockerEntry["Layers"] = json::value_from(layers);
    dockerManifest.push_back(dockerEntry);
    
    std::ofstream manifestOut(destPath + "/manifest.json");
    manifestOut << marshal(dockerManifest);
    manifestOut.close();
    
    json::object repositories;
    json::object imageTags;
    std::string configDigest = dockerEntry["Config"].as_string().c_str(); // 需要显式转换
    repositories[newImageName] = json::object();
    repositories.at(newImageName).as_object()["latest"] = dockerEntry.at("Config").as_string();

    std::ofstream repoOut(destPath + "/repositories");
    repoOut << marshal(repositories);
    repoOut.close();

    //fs::copy_file(configPath, destPath + "/" + dockerEntry["Config"].as_string().c_str(), fs::copy_options::overwrite_existing);
    for (const auto &layer : layerPaths) {
        //std::string layerDest = destPath + "/" + layer.substr(layer.find_last_of('/') + 1);
        std::string layerDest = destPath + "/" + layer.substr(layer.find_last_of('/') + 1);
        if (!fs::exists(layerDest)) 
        {
            fs::create_directories(layerDest);
        }
        std::string layerFile = layerDest + "/layer.tar";
        if (fs::exists(layerFile)) 
        {
            fs::remove(layerFile); // 避免目标路径是文件而不是目录
        }
        convertOciConfigToDockerLayerConfig(configPath, layerDest+"/JSON", configDigest);
        //TODO 做一个version文件
        std::ofstream versionOut(layerDest+ "/" +"version");
        versionOut << "1.0";
        versionOut.close();
        fs::copy_file(layer, layerDest+ "/" +"layer.tar", fs::copy_options::overwrite_existing);
    }

    // 6. 更新index.json
    // std::string new_index_path = destSpec + "/index.json";
    
    // auto new_image = std::make_shared<storage::Image>();
    // auto newImage_index = std::make_shared<storage::manifest>();
    // newImage_index->mediaType = MediaTypeImageManifest;
    // newImage_index->digest = index.manifests[manifest_index].digest;
    // newImage_index->annotations["org.opencontainers.image.ref.name"] = "localhost/" + newImageName;
    // newImage_index->size = index.manifests[manifest_index].size;
    // new_image->image_index = newImage_index;

    // auto images = std::make_shared<ImageStore>();
    // images->dir = destSpec;
    // images->images.push_back(new_image);
    // // 创建新的index.json
    // if (!fs::exists(new_index_path))
    // {
    //     images->Save();
    // }
    // // 更新原有的index.json
    // else
    // {
    //     boost::filesystem::ifstream newIndexfile(new_index_path, std::ios::binary);
    //     std::ostringstream newIndexBuffer;
    //     newIndexBuffer << newIndexfile.rdbuf();
    //     std::string newIndexContent = newIndexBuffer.str();
    //     auto index = unmarshal<storage::index>(newIndexContent);
    //     for (int i = 0; i < index.manifests.size(); i++)
    //     {
    //         if (index.manifests[i].annotations["org.opencontainers.image.ref.name"] == "localhost/" + newImageName)
    //             continue;
            
    //         auto tmpImage = std::make_shared<storage::Image>();
    //         auto tmp = std::make_shared<storage::manifest>();
    //         tmp->mediaType = index.manifests[i].mediaType;
    //         tmp->digest = index.manifests[i].digest;
    //         tmp->annotations["org.opencontainers.image.ref.name"] = index.manifests[i].annotations["org.opencontainers.image.ref.name"];
    //         tmp->size = index.manifests[i].size;
    //         tmpImage->image_index = tmp;
    //         images->images.push_back(tmpImage);
    //     }
    //     images->Save();
    // }
    createTar(iopts->output,destSpec);
    // 删除压缩缓存
    fs::remove_all(destSpec);
    //if(!fs::exists(destSpec))cout << "Error delete!"<<endl;
    delete iopts;
    std::cout << "Save to local dir success!!" << std::endl;
}

