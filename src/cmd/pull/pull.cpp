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
#include "cmd/pull/pull.h"
#include "utils/cli/cli/common.h"
#include "image/image_types/v1/mediatype.h"
#include "image/buildah/tar.h"
#include "utils/logger/ProcessSafeLogger.h"
/**
 * @brief 初始化 pull 命令的内容
 * @details 配置 pull 命令的元信息（名称、描述、示例）及命令行选项：
 *          - `--all-tags`: 拉取镜像的所有标签
 *          - `--os`: 指定目标操作系统（默认 linux）
 *          - `--arch`: 指定目标架构（默认 amd64）
 *          - `--format`: 指定 manifest 格式（oci/v2s1/v2s2）
 * @note 命令示例：
 *       - `ocibuilder pull imagename`
 *       - `ocibuilder pull docker-daemon:imagename:imagetag`
 */
void init_pull(){
    auto  options=std::make_shared <pullOptions>();
    string name{"pull"};
    string Short{"Pull an image from the specified location"};
    string Long{"Pulls an image from a registry and stores it locally.\n\tAn image can be pulled using its tag or digest. If a tag is not\n\tspecified, the image with the 'latest' tag (if it exists) is pulled."};
    string example{"ocibuilder pull imagename\n  ocibuilder pull docker-daemon:imagename:imagetag\n  ocibuilder pull myregistry/myrepository/imagename:imagetag"};
    auto pullCommand=std::make_shared<Command>(name,Short,Long,example);
    string Template=UsageTemplate();
    pullCommand->SetUsageTemplate(Template);
    auto flags=pullCommand->Flags();
    flags->SetInterspersed(false);
    //初始化每个flag的内容
    flags->BoolVar(options->allTags,"all-tags",false,"pull all tags for the specified image");
    flags->String("os","linux","set the OS of the image (e.g., linux, windows) (default unset)");
    flags->String("arch","amd64","set the architecture of the image (e.g., amd64, arm, ppc64le) (default unset)");
    flags->StringVar(options->format, "format", std::string(), "manifest type (oci, v2s1, or v2s2) to use in the destination (default is manifest type of source, with fallbacks)");
    pullCommand->Run=[=](std::shared_ptr<Command> cmd, vector<string> args){
        pullCmd(cmd,args,options);
    };
    rootcmd->AddCommand({pullCommand});
    // return imagesCommand;
}

/**
 * @brief 执行 pull 命令的核心逻辑
 * @param cmd Command 对象，提供上下文和标志位访问
 * @param args 命令行参数，需至少包含一个参数（镜像名称或路径）
 * @param iopts pull 命令选项，包括是否拉取所有标签等
 * @details 根据参数分为两种拉取模式：
 *          1. **远程拉取**：从 registry 下载镜像并存储到本地仓库
 *             - 支持 `--all-tags` 批量拉取
 *             - 自动处理认证（Bearer Token）
 *          2. **本地拉取**：从指定文件夹加载 OCI 镜像文件到仓库
 * @note 异常处理：
 *       - 镜像不存在时输出错误信息并退出
 *       - 文件操作失败时终止流程
 */
void pullCmd(std::shared_ptr<Command> cmd, vector<string> args,std::shared_ptr<pullOptions> iopts){
    logger->set_module("pull");
    std::string src;
    if(args.size()==0){
        logger->log_error("No image specified for pull");
        std::cout<<"Please input the image you want to push!!"<<"\n";
        return;
    } else{
        src=args[0];
    }
    auto store=getStore(cmd);
    auto tmp=cmd->flags->actual_flags;
    std::string allTagFlag="false";
    std::string os="linux";
    std::string arch="amd64";

    // 在拉取时直接从远端拉取原格式
    // bool v1_format = false;
    if(tmp.find("os")!=tmp.end()){
        os=tmp["os"]->value->String();
    }
    if(tmp.find("all-tags")!=tmp.end()){
        allTagFlag=tmp["all-tags"]->value->String();
    }
    if(tmp.find("arch")!=tmp.end()){
        arch=tmp["arch"]->value->String();
    }
    // if (tmp.find("format") != tmp.end()) {
    //     v1_format = true;
    //     std::cout << "true" << std::endl;
    //     iopts->format = tmp["format"]->value->String();
    // }


    logger->log_info("Starting pull operation for image: " + src);
    dockerClient client;
    auto url=client.resolveRequestURL(src);

    //从远端库中拉取
    if(!url->localPullFlag){
        loadLoginInfo(url->host+":"+url->port);
        loginAuth.cookie.erase();
        loginAuth.bearerToken.erase();
        logger->log_info("Authenticating with registry: " + url->host + ":" + url->port);
        std::string btoken_push = login_and_getToken(userinfo.username, userinfo.password, url->host, url->port, url->projectName, url->imageName);
        if (!btoken_push.empty()) {
            logger->log_info("Authentication successful");
            loginAuth.bearerToken = btoken_push;
        } else {
            logger->log_error("Authentication failed");
            loginAuth.bearerToken.erase();
        }
        
        //获得bearer token
        // std::string token=getToken(url->host,url->port,url->projectName,url->imageName);
        //处理 all-tags 参数
        if(allTagFlag=="true"){
            std::vector<std::string> tagList = getTagList(url->host,url->port,url->projectName,url->imageName);

            for(size_t i=0;i<tagList.size();i++){
                std::string digest="";
                size_t mlen=0;
                std::tie(digest,mlen)=pullManifestAndBlob(url->host,url->port,url->projectName,url->imageName,tagList[i],os,arch);
            if(digest == "" || mlen == 0){
                logger->log_error("Failed to pull manifest");
                std::cout<<"pull manifest error"<<std::endl;
                return;
                }
                auto images=std::dynamic_pointer_cast<ImageStore>(store->image_store);
            if(images==nullptr){
                logger->log_error("Image store is null");
                std::cerr<<"imageindexs error"<<std::endl;
                }
                auto newindex=std::make_shared<storage::manifest>();
                std::string name =url->imageName+":"+tagList[i];
                newindex->mediaType= MediaTypeImageManifest;
                newindex->digest=digest;
                newindex->annotations["org.opencontainers.image.ref.name"]="localhost/"+name;
                newindex->size=mlen;
                auto image=images->lookup(name);

                if(image!=nullptr){
                    newindex->historyTags=image->image_index->historyTags;
                    std::string digest=newindex->digest;
                    if(digest!=image->image_index->digest){
                        newindex->historyTags.push_back(image->image_index->digest);
                    }
                    auto it=find_if(newindex->historyTags.begin(),newindex->historyTags.end(),[digest](std::string d){
                        return digest==d;
                    });
                    if(it!=newindex->historyTags.end()){
                        newindex->historyTags.erase(it);
                    }
                    image->image_index=newindex;
                }else{
                    auto newimage=std::make_shared<storage::Image>();
                    newimage->image_index=newindex;
                    images->images.push_back(newimage);
                }
            images->Save();
            
            //从文件读取manifest
            boost::filesystem::ifstream manifestfile("oci_images/oci_registry/blobs/sha256/"+digest.substr(7),std::ios::binary);
            std::ostringstream buffer1;
            buffer1 << manifestfile.rdbuf();
            std::string fileContent1 = buffer1.str();
            auto manifest=unmarshal<::Manifest>(fileContent1);
            //从文件读config
            boost::filesystem::ifstream configfile("oci_images/oci_registry/blobs/sha256/"+manifest.Config.Digests.digest.substr(7),std::ios::binary);
            std::ostringstream buffer2;
            buffer2 << configfile.rdbuf();
            std::string fileContent2 = buffer2.str();
            auto config=unmarshal<v1::Image>(fileContent2);

            auto Layers = manifest.Layers;
            auto layerStore = store->getLayerStoreLocked();
        if (!layerStore) {
            //logger->log_error("Failed to get layer store");
            return ;
        }
        auto layerOptions=std::make_shared<LayerOptions>();
        
        for(size_t j=0; j<Layers.size(); j++) {
            std::string id = config.rootFS.diffIDs[i].substr(7);
            std::string blobPath = "oci_images/oci_registry/blobs/sha256/" + 
            Layers[j].Digests.digest.substr(7);
            std::ifstream diff(blobPath, std::ios::binary);
            std::shared_ptr<Layer> parentLayer = nullptr;
            // if(i > 0) {
            //     parentLayer = layerStore->lookup(manifest.Layers[i-1].Digests.digest);
            // }
            
            //std::vector<std::string> names = {imageName+":"+version};
            std::map<std::string, std::string> options;
            layerStore->create(
                id,
                parentLayer,
                {},//names,
                "",
                options,
                layerOptions,
                true,
                diff
            );
            std::string outputFilePath = "oci_images/overlay/"+id+"/diff";

            if (extractTarGz(blobPath, outputFilePath,config.rootFS.diffIDs[j].substr(7))) {
                logger->log_info("Successfully extracted tar.gz");
                std::cout << "extract targz successful" << std::endl;
            } else {
                logger->log_error("Failed to extract tar.gz");
                std::cout << "extract targz failed" << std::endl;
            }
            layerStore->savelayer();
        }
            }
        } else{
            std::string digest="";
            size_t mlen=0;
            std::tie(digest,mlen)=pullManifestAndBlob(url->host,url->port,url->projectName,url->imageName,url->version,os,arch);
            if(digest == "" || mlen == 0){
                logger->log_error("Failed to pull manifest");
                std::cout<<"pull manifest error"<<std::endl;
                return;
            }
            auto images=std::dynamic_pointer_cast<ImageStore>(store->image_store);
            if(images==nullptr){
                logger->log_error("Image store is null");
                std::cerr<<"imageindexs error"<<std::endl;
            }
            auto newindex=std::make_shared<storage::manifest>();
            std::string name =url->imageName+":"+url->version;
            newindex->mediaType= MediaTypeImageManifest;
            newindex->digest=digest;
            newindex->annotations["org.opencontainers.image.ref.name"]="localhost/"+name;
            newindex->size=mlen;
            auto image=images->lookup(name);

            if(image!=nullptr){
                newindex->historyTags=image->image_index->historyTags;
                std::string digest=newindex->digest;
                if(digest!=image->image_index->digest){
                    newindex->historyTags.push_back(image->image_index->digest);
                }          
                auto it=find_if(newindex->historyTags.begin(),newindex->historyTags.end(),[digest](std::string d){
                    return digest==d;
                });
                if(it!=newindex->historyTags.end()){
                    newindex->historyTags.erase(it);
                }
                image->image_index=newindex;
            }else{
                auto newimage=std::make_shared<storage::Image>();
                newimage->image_index=newindex;
                images->images.push_back(newimage);
            }
            images->Save();
            //从文件读取manifest
            boost::filesystem::ifstream manifestfile("oci_images/oci_registry/blobs/sha256/"+digest.substr(7),std::ios::binary);
            std::ostringstream buffer1;
            buffer1 << manifestfile.rdbuf();
            std::string fileContent1 = buffer1.str();
            auto manifest=unmarshal<::Manifest>(fileContent1);

            //从文件读config
            boost::filesystem::ifstream configfile("oci_images/oci_registry/blobs/sha256/"+manifest.Config.Digests.digest.substr(7),std::ios::binary);
            std::ostringstream buffer2;
            buffer2 << configfile.rdbuf();
            std::string fileContent2 = buffer2.str();
            auto config=unmarshal<v1::Image>(fileContent2);

            auto Layers = manifest.Layers;
            auto layerStore = store->getLayerStoreLocked();
        if (!layerStore) {
            //logger->log_error("Failed to get layer store");
            return ;
        }
        auto layerOptions=std::make_shared<LayerOptions>();
        
        for(size_t i=0; i<Layers.size(); i++) {
            std::string id = config.rootFS.diffIDs[i].substr(7);
            std::string blobPath = "oci_images/oci_registry/blobs/sha256/" + 
            Layers[i].Digests.digest.substr(7);
            std::ifstream diff(blobPath, std::ios::binary);
            std::shared_ptr<Layer> parentLayer = nullptr;
            // if(i > 0) {
            //     parentLayer = layerStore->lookup(manifest.Layers[i-1].Digests.digest);
            // }
            
            //std::vector<std::string> names = {imageName+":"+version};
            std::map<std::string, std::string> options;
            layerStore->create(
                id,
                parentLayer,
                {},//names,
                "",
                options,
                layerOptions,
                true,
                diff
            );
            std::string outputFilePath = "oci_images/overlay/"+id+"/diff";
            if (extractTarGz(blobPath, outputFilePath,config.rootFS.diffIDs[i].substr(7))){
                logger->log_info("Successfully extracted tar.gz");
                std::cout << "extract targz successful" << std::endl;
            } else {
                logger->log_error("Failed to extract tar.gz");
                std::cout << "extract targz failed" << std::endl;
            }
            layerStore->savelayer();
        }

        }

    }else{//如果是从指定文件夹中拉取
        //本地库路径
        std::string destPath = "oci_images/oci_registry/blobs/sha256";
        std::string indexPath = url->localPullPath+"/index.json";
        if(!fs::exists(indexPath)){
            logger->log_error("Index file does not exist: " + indexPath);
            std::cout << "File does not exist: " << indexPath << std::endl;
            return;
        }
        if (!fs::exists(destPath)) {
            logger->log_info("Creating destination directory: " + destPath);
            fs::create_directories(destPath);
        }
        boost::filesystem::ifstream indexfile(indexPath,std::ios::binary);
        std::ostringstream buffer;
        buffer << indexfile.rdbuf();
        std::string fileContent = buffer.str();
        auto index=unmarshal<storage::index>(fileContent);
        std::string imageNAV = "localhost/"+url->imageName+":"+url->version;
        // if(index.manifests[0].annotations["org.opencontainers.image.ref.name"]!=imageNAV){
        //     std::cout << "Invalid imageName or Version" << std::endl;
        //     return;
        // }

        std::string manifestPath = url->localPullPath+"/blobs/sha256/"+index.manifests[0].digest.substr(7);
        if(!fs::exists(manifestPath)){
            logger->log_error("Manifest file does not exist");
            std::cout << "ManifestFile does not exist: " << std::endl;
            return;
        }

        boost::filesystem::ifstream manifestfile(manifestPath,std::ios::binary);
        std::ostringstream buffer1;
        buffer1 << manifestfile.rdbuf();
        std::string fileContent1 = buffer1.str();
        auto manifest=unmarshal<::Manifest>(fileContent1);

        manifest.MediaType ="application/vnd.docker.distribution.manifest.v2+json";
        manifest.Config.MediaType = "application/vnd.docker.container.image.v1+json";
        for (auto& layer : manifest.Layers) {
            layer.MediaType = "application/vnd.docker.image.rootfs.diff.tar.gzip";
        }

        std::string configPath = url->localPullPath+"/blobs/sha256/"+manifest.Config.Digests.digest.substr(7);
        if(!fs::exists(configPath)){
            logger->log_error("Config file does not exist");
            std::cout << "Config does not exist: " << std::endl;
            return;
        }

        std::vector<std::string> blobPath;
        for(size_t i=0;i<manifest.Layers.size();i++){
            std::string tmp=url->localPullPath+"/blobs/sha256/"+manifest.Layers[i].Digests.digest.substr(7);
            if(!fs::exists(tmp)){
            logger->log_error("Blob file does not exist");
            std::cout << "Blob does not exist: " << std::endl;
                return;
            }
            blobPath.push_back(tmp);
        }

        //文件全部存在 依次copy
        // fs::copy_file(manifestPath,destPath+"/"+index.manifests[0].digest.substr(7),fs::copy_options::overwrite_existing);
        std::ofstream manifestOut(destPath+"/"+index.manifests[0].digest.substr(7));
        manifestOut << marshal(manifest);
        manifestOut.close();
        fs::copy_file(configPath,destPath+"/"+manifest.Config.Digests.digest.substr(7),fs::copy_options::overwrite_existing);

        for(size_t i=0;i<manifest.Layers.size();i++){
            std::string blobsha=manifest.Layers[i].Digests.digest.substr(7);
            std::string blobP=url->localPullPath+"/blobs/sha256/"+blobsha;
            logger->log_info("Copying blob: " + blobsha);
            fs::copy_file(blobP,destPath+"/"+blobsha,fs::copy_options::overwrite_existing);
        }

        //处理index.json
        auto images=std::dynamic_pointer_cast<ImageStore>(store->image_store);
        if(images==nullptr){
            logger->log_error("Image store is null");
            std::cerr<<"imageindexs error"<<std::endl;
        }
        
        std::string name =url->imageName+":"+url->version;
        auto image=images->lookup(name);

        auto newindex=std::make_shared<storage::manifest>();
        newindex->mediaType= MediaTypeImageManifest;
        newindex->digest=index.manifests[0].digest;
        newindex->annotations["org.opencontainers.image.ref.name"]="localhost/"+name;
        newindex->size=index.manifests[0].size;
        if(image!=nullptr){
            newindex->historyTags=image->image_index->historyTags;
            std::string digest=newindex->digest;
            if(digest!=image->image_index->digest){
                newindex->historyTags.push_back(image->image_index->digest);
            }
            auto it=find_if(newindex->historyTags.begin(),newindex->historyTags.end(),[digest](std::string d){
                return digest==d;
            });
            if(it!=newindex->historyTags.end()){
                newindex->historyTags.erase(it);
            }
            image->image_index=newindex;
        }else{
            auto newimage=std::make_shared<storage::Image>();
            newimage->image_index=newindex;
            images->images.push_back(newimage);
        }
        images->Save();
        //从文件读config
        boost::filesystem::ifstream configfile(configPath,std::ios::binary);
        std::ostringstream buffer2;
        buffer2 << configfile.rdbuf();
        std::string fileContent2 = buffer2.str();
        auto config=unmarshal<v1::Image>(fileContent2);
        
        auto layerStore = store->getLayerStoreLocked();
        if (!layerStore) {
            //logger->log_error("Failed to get layer store");
            return ;
        }
        auto layerOptions=std::make_shared<LayerOptions>();
        
        for(size_t i=0; i<manifest.Layers.size(); i++) {
            std::string id = config.rootFS.diffIDs[i].substr(7);
            std::string blobPath = "oci_images/oci_registry/blobs/sha256/" + 
            manifest.Layers[i].Digests.digest.substr(7);
            std::ifstream diff(blobPath, std::ios::binary);
            std::shared_ptr<Layer> parentLayer = nullptr;
            // if(i > 0) {
            //     parentLayer = layerStore->lookup(manifest.Layers[i-1].Digests.digest);
            // }
            
            //std::vector<std::string> names = {imageName+":"+version};
            std::map<std::string, std::string> options;
            layerStore->create(
                id,
                parentLayer,
                {},//names,
                "",
                options,
                layerOptions,
                true,
                diff
            );
            std::string outputFilePath = "oci_images/overlay/"+id+"/diff";
            if (extractTarGz(blobPath, outputFilePath,config.rootFS.diffIDs[i].substr(7))) {
                logger->log_info("Successfully extracted tar.gz");
                std::cout << "extract targz successful" << std::endl;
            } else {
                logger->log_error("Failed to extract tar.gz");
                std::cout << "extract targz failed" << std::endl;
            }
            layerStore->savelayer();
        }
    }
    logger->log_info("Pull operation completed successfully");
    // delete iopts;
}
