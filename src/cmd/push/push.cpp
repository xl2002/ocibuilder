/**
 * @file push.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 push.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "cmd/push/push.h"
#include "image/buildah/retry.h"
#include "network/network.h"
#include "utils/cli/cli/common.h"
#include "security/auth.h"
#include "utils/common/go/string.h"
#include "utils/common/go/file.h"
#include "image/digest/digest.h"
#include "cmd/login/login.h"
#include "image/image_types/v1/oci.h"
#include "image/image_types/v1/config.h"


/**
 * @brief 初始化 push 命令的内容
 * @details 配置 push 命令的元信息（名称、描述、示例）及命令行选项：
 *          - `--format`: 指定 manifest 格式（oci/v2s1/v2s2）
 *          - `--authfile`: 指定认证文件路径
 * @note 命令支持两种调用方式：
 *       - 远程推送：`ocibuilder push registry/image:tag`
 *       - 本地推送：`ocibuilder push src dst`
 */
void init_push()
{
    auto opts = new pushOptions();
    string name{"push"};
    string Short{"Push an image to a specified destination"};
    string Long{"Pushes an image to a specified location."};
    string example{"ocibuilder pull imagename\n  ocibuilder pull docker-daemon:imagename:imagetag\n  ocibuilder pull myregistry/myrepository/imagename:imagetag"};
    Command *pushCommand = new Command{name, Short, Long, example};
    string Template = UsageTemplate();
    pushCommand->SetUsageTemplate(Template);

    Flagset *flags = pushCommand->Flags();
    // flags.StringVar();
    flags->SetInterspersed(false);
    flags->StringVar(opts->format, "format", std::string(), "manifest type (oci, v2s1, or v2s2) to use in the destination (default is manifest type of source, with fallbacks)");
    flags->StringVar(opts->authfile, "authfile", std::string(), "path of the authentication file. Use REGISTRY_AUTH_FILE environment variable to override");
    pushCommand->Run = [=](Command &cmd, vector<string> args)
    {
        // 有可能有format参数
        if (args.size() == 1 || args.size() == 3)
            pushCmd(cmd, args, opts);
        else if (args.size() == 2 || args.size() == 4)
            pushCmdLocal(cmd, args, opts);
        else if (args.size() == 0)
            std::cerr << "Please input the image you want to push!!" << std::endl;
        else
            std::cerr << "the number of arguments is not right!!" << std::endl;
    };
    rootcmd.AddCommand({pushCommand});
    // return imagesCommand;
}

/**
 * @brief 执行远程镜像推送的核心逻辑
 * @param cmd Command 对象，提供上下文和标志位访问
 * @param args 命令行参数，需包含目标镜像地址
 * @param iopts push 命令选项，包括 manifest 格式等
 * @details 执行流程：
 *          1. 认证检查（Bearer Token 获取）
 *          2. 解析镜像地址（registry/repo:tag）
 *          3. 分步上传：
 *             - Blobs 层数据（并行校验+断点续传）
 *             - Config 文件
 *             - Manifest 文件
 * @warning 需确保网络连通性和仓库写入权限
 * @return 无返回值，失败时直接终止进程
 */
void pushCmd(Command &cmd, vector<string> args, pushOptions *iopts)
{
    // 执行流程
    std::string src;
    // 1. 获得令牌
    CheckAuthFile(iopts);
    
    // 判断使用哪种格式
    bool v1_format = false;

    auto tmp = cmd.flags->actual_flags;
    if (tmp.find("format") != tmp.end())
    {
        v1_format = true;
        iopts->format = tmp["format"]->value->String();
    }

    // // 判断是否存在--format参数
    // if (args.size() == 1)
    // {
    //     src = args[0];
    // }
    // else if (args.size() == 3)
    // {
    //     // 以后如果使用其他格式，可以进行进一步扩展
    //     if (args[0] == "--format") {
    //         src = args[2];
    //         iopts->format = args[1];
    //     }
    //     else {
    //         src = args[0];
    //         iopts->format = args[2];
    //     }
    // }
    src = args[0];
    auto compress = compression::Gzip;
    // 读取本地镜像的数据
    auto store = getStore(&cmd);

    // 分析镜像名
    std::string urlpath, withinTransport;
    std::tie(urlpath, withinTransport, std::ignore) = Cut(src, '/');
    std::tie(std::ignore, withinTransport, std::ignore) = Cut(withinTransport, '/');
    auto imagestore = store->Image(withinTransport);

    // 拿到push命令中的这一部分

    // std::string image="localhost:5000/busybox:latest";
    // 解析出来host、port、镜像名等
    dockerClient client;
    auto url = client.resolveRequestURL(src);
    //如果http/https
    // if (url->scheme == "http" || url->scheme == "https") {
        // 如果是http
        //字符串匹配判断ip/url
        //如果是url，DNS解析

    // }
    //如果没有，字符串匹配判断ip/url
    //如果是url，DNS解析,用https
    //如果是ip，用http

    
    //  执行登录请求
    loadLoginInfo(url->host + ":" + url-> port);
    // std::string btoken = login_and_getToken(userinfo.username, userinfo.password, url->host, url->port, "", "");
    // // 将从harbor库获取的bearer token存储到dockerClient中
    // if (!btoken.empty())
    //     loginAuth.bearerToken = btoken;
    // else
    //     loginAuth.bearerToken.erase();
    // bool flag = login(url->host, url->port, userinfo.username, userinfo.password);
    // if (!flag) {
    //     std::cerr << "fail to login!!" << "\n";
    //     return;
    // }
    loginAuth.cookie.erase();
    loginAuth.bearerToken.erase();

    // 在执行push之前需要再次完成登录请求
    std::string btoken_push = login_and_getToken(userinfo.username, userinfo.password, url->host, url->port, url->projectName, url->imageName);
        // 将从harbor库获取的bearer token存储到dockerClient中
    if (!btoken_push.empty())
        loginAuth.bearerToken = btoken_push;
    else
    {
        loginAuth.bearerToken.erase();
    }

    // if (!ifSupportV2(url->host, url->port))
    // {
    //     std::cout << "Can't push!!" << "\n";
    // }
    // 读取本地镜像层的数据
    // 先上传blobs数据层
    auto layers = imagestore->image_manifest->Layers;
    auto blobsNum = layers.size();
    // 依次上传每层blob
    std::string uid;
    std::string state;
    bool allBlobsUploaded = true;  // 标记所有blob是否上传成功

    for (int i = 0; i < blobsNum; i++)
    {
        // 拿到每层数据data的路径还有hash值
        std::string shaId = layers[i].Digests.Encoded();
        std::string filePath = store->image_store_dir + "/blobs/sha256/" + shaId;
        if (!isCorrect(shaId, filePath))
        {
            std::cerr << "the blob: " + shaId + " is not correct!!" << std::endl;
            allBlobsUploaded = false;
            break;
        }
        std::string fisrtTwoC = shaId.substr(0, 2);
        // 判断这层数据是否在服务器存在，不存在再传输
        if (!ifBlobExists(url->host, url->port, url->imageName, shaId, url->projectName))
        {
            // 创建重试配置
            auto retryOptions = std::make_shared<Retry::Options>();
            retryOptions->MaxRetry = 3;
            retryOptions->Delay = std::chrono::seconds(1);

            // 初始化上传(带重试)
            std::pair<std::string, std::string> initResult;

                initResult = initUpload(url->host, url->port, url->imageName, url->projectName);
                if (initResult.first.empty() || initResult.second.empty()) {
                    throw std::runtime_error("Failed to initialize upload");
                }

            uid = initResult.first;
            state = initResult.second;
            
            // 拿到data数据大小
            std::ifstream file(filePath, std::ios::binary | std::ios::ate);
            std::size_t total_size = file.tellg();
            file.close();
            
            // 上传数据(带重试)
            std::pair<std::string, std::string> uploadResult;
            RetryIfNecessary([&](){
                uploadResult = uploadBlobChunk(url->host, url->port, uid, state, filePath, 0, total_size, total_size, url->imageName, url->projectName);
                if (uploadResult.first.empty() || uploadResult.second.empty()) {
                    throw std::runtime_error("Failed to upload blob chunk");
                }
            }, retryOptions);
            
            uid = uploadResult.first;
            state = uploadResult.second;
            
            // 完成上传(带重试)
            RetryIfNecessary([&](){
                finalizeUpload(url->host, url->port, uid, shaId, state, url->imageName, url->projectName);
                if (!ifBlobExists(url->host, url->port, url->imageName, shaId, url->projectName)) {
                    throw std::runtime_error("Blob upload verification failed");
                }
            }, retryOptions);
            // 上传完成后再次检查blob是否存在
            if (!ifBlobExists(url->host, url->port, url->imageName, shaId, url->projectName)) {
                std::cerr << "Blob upload verification failed: " << shaId << " not found in repository" << std::endl;
                allBlobsUploaded = false;
                break;
            }
        }
    }
    // 如果blob上传失败，直接返回
    if (!allBlobsUploaded) {
        std::cerr << "Failed to push some blobs, aborting manifest upload" << std::endl;
        delete iopts;
        return;
    }

    // 再上传config数据
    std::string shaId1 = imagestore->image_manifest->Config.Digests.Encoded();
    std::string configPath = store->image_store_dir + "/blobs/sha256/" + shaId1;
    if (!isCorrect(shaId1, configPath))
    {
        std::cerr << "the config: " + shaId1 + " is not correct!!" << std::endl;
        return;
    }
    std::string fisrtTwoC = shaId1.substr(0, 2);
    // 判断这层数据是否在服务器存在，不存在再传输
    if (!ifBlobExists(url->host, url->port, url->imageName, shaId1, url->projectName))
    {
        std::pair<std::string, std::string> initResult = initUpload(url->host, url->port, url->imageName, url->projectName);
        if (initResult.first.empty() || initResult.second.empty()) {
            std::cerr << "Failed to initialize upload for config" << std::endl;
            delete iopts;
            return;
        }
        uid = initResult.first;
        state = initResult.second;

        // 拿到data数据大小
        std::ifstream file(configPath, std::ios::binary | std::ios::ate);
        std::size_t total_size = file.tellg();
        file.close();
        // 上传数据
        std::pair<std::string, std::string> uploadResult = uploadBlobChunk(url->host, url->port, uid, state, configPath, 0, total_size, total_size, url->imageName, url->projectName);
        uid = uploadResult.first;
        state = uploadResult.second;
        // 完成本次上传
        finalizeUpload(url->host, url->port, uid, shaId1, state, url->imageName, url->projectName);
        // 上传完成后再次检查config是否存在
        if (!ifBlobExists(url->host, url->port, url->imageName, shaId1, url->projectName)) {
            std::cerr << "Config upload verification failed: " << shaId1 << " not found in repository" << std::endl;
            delete iopts;
            return;
        }
    }

    // 最后上传manifest数据
    std::string shaId2 = imagestore->digest->Encoded();
    std::string manifestPath = store->image_store_dir + "/blobs/sha256/" + shaId2;
    if (!isCorrect(shaId2, manifestPath))
    {
        std::cerr << "the manifest: " + shaId2 + " is not correct!!" << std::endl;
        return;
    }
    std::string manifestType;
    if (!v1_format)
        manifestType = imagestore->image_index->mediaType;
    else
        manifestType = "application/vnd.oci.image.manifest.v1+json";
    std::string fisrtTwoC2 = shaId2.substr(0, 2);
    // buildah不用判断manifest是否存在，直接上传
    std::ifstream file(manifestPath, std::ios::binary | std::ios::ate);
    std::size_t total_size = file.tellg();
    file.close();
    // 上传数据
    uploadManifest(url->host, url->port, manifestPath, 0, total_size, url->imageName, url->version, manifestType, url->projectName, v1_format, store->image_store_dir);
    if(!v1_format && !ifManifestExists(url->host, url->port, url->imageName,url->version, url->projectName))
    {
        std::cerr << "Manifest upload verification failed: " << url->imageName<< ":"<<url->version << " not found in repository" << std::endl;
        return;
    }
    // if (!ifBlobExists(url->host, url->port, url->imageName, shaId2, url->projectName))
    // {
    //     // std::pair<std::string, std::string> initResult = initUpload(url->host, url->port, url->imageName);
    //     // std::string uid = initResult.first;
    //     // std::string state = initResult.second;

    //     // 拿到data数据大小
    //     std::ifstream file(manifestPath, std::ios::binary | std::ios::ate);
    //     std::size_t total_size = file.tellg();
    //     file.close();
    //     // 上传数据
    //     uploadManifest(url->host, url->port, manifestPath, 0, total_size, url->imageName, url->version, manifestType, url->projectName);
    // }
    delete iopts;
    std::cout << "Push success!!" << std::endl;
}

/**
 * @brief 从参数字符串中提取并转换路径
 * @param param 格式为 "prefix:path:suffix" 的字符串
 * @return std::string 转换后的绝对路径
 * @throws 无显式抛出，但路径无效时可能触发 filesystem_error
 * @example 
 *  输入 "docker:./path:latest" → 返回 "/abs/path/to/current/dir/path"
 */
std::string extractAndConvertPath(const std::string &param)
{
    // 找到第一个冒号的位置
    size_t firstColon = param.find(':');
    // 找到第二个冒号的位置
    size_t lastColon = param.rfind(':');
    size_t secondLastColon = param.rfind(':', lastColon - 1);
    
    // 提取路径
    std::string path = param.substr(firstColon + 1, secondLastColon - firstColon - 1);
    
    // 转换为绝对路径
    boost::filesystem::path boostPath(path);
    boost::filesystem::path absolutePath = boost::filesystem::absolute(boostPath);
    
    return absolutePath.string();
}
/**
 * @brief 执行本地目录镜像推送
 * @param cmd Command 对象，提供存储库上下文
 * @param args 命令行参数，需包含源镜像名和目标路径
 * @param iopts push 命令选项（当前主要用 authfile）
 * @details 实现功能：
 *          1. 源镜像元数据解析（index.json）
 *          2. 文件系统操作：
 *             - 创建目标目录结构
 *             - 拷贝 blob/config/manifest 文件
 *             - 生成/更新目标 index.json
 * @note 适用于离线环境镜像迁移
 */
void pushCmdLocal(Command &cmd, vector<string> args, pushOptions * iopts)
{
    std::string destPath, withinTransport, destSpec, newImageName;
    // 1. 获得令牌
    CheckAuthFile(iopts);
    // 镜像名
    withinTransport = args[0];
    destPath = args[1];

    // 读取本地镜像的数据
    auto store = getStore(&cmd);
    // 获得index.json
    std::string indexPath = store.get()->image_store_dir + "/index.json";

    // 得到或创建目标路径
    destSpec = extractAndConvertPath(destPath);
    size_t lastColon = destPath.rfind(':');
    size_t secondLastColon = destPath.rfind(':', lastColon - 1);
    newImageName = destPath.substr(secondLastColon +1);
    // destSpec = "C:\\Users\\admin\\Documents\\output";
    // std::tie(std::ignore, destPath, std::ignore) = Cut(destPath, ':');
    destPath = destSpec + "/blobs/sha256";
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
    DockerManifest docker_manifest;

    manifest.MediaType ="application/vnd.oci.image.manifest.v1+json";
    manifest.Config.MediaType = "application/vnd.oci.image.config.v1+json";
    for (auto& layer : manifest.Layers) {
        layer.MediaType = "application/vnd.oci.image.layer.v1.tar+gzip";
    }
    for (const auto& pair : index.manifests[manifest_index].annotations) {
        manifest.Annotations[pair.first] = pair.second;
    }

    docker_manifest.SchemaVersion = manifest.SchemaVersion;
    docker_manifest.MediaType = manifest.MediaType;
    docker_manifest.Config = manifest.Config;
    docker_manifest.Layers = manifest.Layers;
    docker_manifest.Annotations = manifest.Annotations;

    // 3. 找到config文件
    std::string configPath = store.get()->image_store_dir + "/blobs/sha256/" + manifest.Config.Digests.digest.substr(7);
    if (!fs::exists(configPath))
    {
        std::cerr << "Config file not found" << std::endl;
        return;
    }

    // 4. 找到blobs文件
    std::vector<std::string> blobPath;
    for (int i = 0; i < manifest.Layers.size(); i++)
    {
        std::string tmp = store.get()->image_store_dir + "/blobs/sha256/" + manifest.Layers[i].Digests.digest.substr(7);
        if (!fs::exists(tmp))
        {
            std::cout << "Blob does not exist: " << std::endl;
            return;
        }
        blobPath.push_back(tmp);
    }

    // 5. push到新的目录下
    // fs::copy_file(manifestPath, destPath + "/" + index.manifests[manifest_index].digest.substr(7), fs::copy_options::overwrite_existing);
    std::ofstream manifestOut(destPath + "/" + index.manifests[manifest_index].digest.substr(7));
    manifestOut << marshal(docker_manifest);
    manifestOut.close();
    fs::copy_file(configPath, destPath + "/" + manifest.Config.Digests.digest.substr(7), fs::copy_options::overwrite_existing);
    for (int i = 0; i < blobPath.size(); i++)
    {
        fs::copy_file(blobPath[i], destPath + "/" + manifest.Layers[i].Digests.digest.substr(7), fs::copy_options::overwrite_existing);
    }

    // 6. 更新index.json
    std::string new_index_path = destSpec + "/index.json";
    
    auto new_image = std::make_shared<storage::Image>();
    auto newImage_index = std::make_shared<storage::manifest>();
    newImage_index->mediaType = "application/vnd.oci.image.manifest.v1+json";
    newImage_index->digest = index.manifests[manifest_index].digest;
    newImage_index->annotations["org.opencontainers.image.ref.name"] = "localhost/" + newImageName;
    newImage_index->size = index.manifests[manifest_index].size;
    new_image->image_index = newImage_index;

    auto images = std::make_shared<ImageStore>();
    images->dir = destSpec;
    images->images.push_back(new_image);
    // 创建新的index.json
    if (!fs::exists(new_index_path))
    {
        images->Save();
    }
    // 更新原有的index.json
    else
    {
        boost::filesystem::ifstream newIndexfile(new_index_path, std::ios::binary);
        std::ostringstream newIndexBuffer;
        newIndexBuffer << newIndexfile.rdbuf();
        std::string newIndexContent = newIndexBuffer.str();
        auto index = unmarshal<storage::index>(newIndexContent);
        for (int i = 0; i < index.manifests.size(); i++)
        {
            if (index.manifests[i].annotations["org.opencontainers.image.ref.name"] == "localhost/" + newImageName)
                continue;
            
            auto tmpImage = std::make_shared<storage::Image>();
            auto tmp = std::make_shared<storage::manifest>();
            tmp->mediaType = index.manifests[i].mediaType;
            tmp->digest = index.manifests[i].digest;
            tmp->annotations["org.opencontainers.image.ref.name"] = index.manifests[i].annotations["org.opencontainers.image.ref.name"];
            tmp->size = index.manifests[i].size;
            tmpImage->image_index = tmp;
            images->images.push_back(tmpImage);
        }
        images->Save();
    }
    delete iopts;
    std::cout << "Push local dir success!!" << std::endl;
}
