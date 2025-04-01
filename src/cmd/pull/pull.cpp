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
/**
 * @brief 初始化 pull 命令的内容
 * 
 */
void init_pull(){
    pullOptions* options=new pullOptions();
    string name{"pull"};
    string Short{"Pull an image from the specified location"};
    string Long{"Pulls an image from a registry and stores it locally.\n\tAn image can be pulled using its tag or digest. If a tag is not\n\tspecified, the image with the 'latest' tag (if it exists) is pulled."};
    string example{"ocibuilder pull imagename\n  ocibuilder pull docker-daemon:imagename:imagetag\n  ocibuilder pull myregistry/myrepository/imagename:imagetag"};
    Command* pullCommand=new Command{name,Short,Long,example};
    string Template=UsageTemplate();
    pullCommand->SetUsageTemplate(Template);
    Flagset* flags=pullCommand->Flags();
    flags->SetInterspersed(false);
    //初始化每个flag的内容
    flags->BoolVar(options->allTags,"all-tags",false,"pull all tags for the specified image");
    flags->String("os","linux","set the OS of the image (e.g., linux, windows) (default unset)");
    flags->String("arch","amd64","set the architecture of the image (e.g., amd64, arm, ppc64le) (default unset)");
    flags->StringVar(options->format, "format", std::string(), "manifest type (oci, v2s1, or v2s2) to use in the destination (default is manifest type of source, with fallbacks)");
    pullCommand->Run=[=](Command& cmd, vector<string> args){
        pullCmd(cmd,args,options);
    };
    rootcmd.AddCommand({pullCommand});
    // return imagesCommand;
}

/**
 * @brief pull 命令Run操作的
 * 
 */
void pullCmd(Command& cmd, vector<string> args,pullOptions* iopts){
    std::string src;
    if(args.size()==0){
        std::cout<<"Please input the image you want to push!!"<<"\n";
        return;
    } else{
        src=args[0];
    }
    auto store=getStore(&cmd);
    auto tmp=cmd.flags->actual_flags;
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


    dockerClient client;
    auto url=client.resolveRequestURL(src);

    //从远端库中拉取
    if(!url->localPullFlag){
        loadLoginInfo(url->host+":"+url->port);
        loginAuth.cookie.erase();
        loginAuth.bearerToken.erase();
        std::string btoken_push = login_and_getToken(userinfo.username, userinfo.password, url->host, url->port, url->projectName, url->imageName);
        if (!btoken_push.empty())
            loginAuth.bearerToken = btoken_push;
        else
            loginAuth.bearerToken.erase();
        
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
                    std::cout<<"pull manifest error"<<std::endl;
                    return;
                }
                auto images=std::dynamic_pointer_cast<ImageStore>(store->image_store);
                if(images==nullptr){
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
            }
        } else{
            std::string digest="";
            size_t mlen=0;
            std::tie(digest,mlen)=pullManifestAndBlob(url->host,url->port,url->projectName,url->imageName,url->version,os,arch);
            if(digest == "" || mlen == 0){
                std::cout<<"pull manifest error"<<std::endl;
                return;
            }
            auto images=std::dynamic_pointer_cast<ImageStore>(store->image_store);
            if(images==nullptr){
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
        }

    }else{//如果是从指定文件夹中拉取
        //本地库路径
        std::string destPath = "oci_images/oci_registry/blobs/sha256";
        std::string indexPath = url->localPullPath+"/index.json";
        if(!fs::exists(indexPath)){
            std::cout << "File does not exist: " << indexPath << std::endl;
            return;
        }
        if (!fs::exists(destPath)) {
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
            std::cout << "Config does not exist: " << std::endl;
            return;
        }

        std::vector<std::string> blobPath;
        for(size_t i=0;i<manifest.Layers.size();i++){
            std::string tmp=url->localPullPath+"/blobs/sha256/"+manifest.Layers[i].Digests.digest.substr(7);
            if(!fs::exists(tmp)){
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
            fs::copy_file(blobP,destPath+"/"+blobsha,fs::copy_options::overwrite_existing);
        }

        //处理index.json
        auto images=std::dynamic_pointer_cast<ImageStore>(store->image_store);
        if(images==nullptr){
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
        
    }
    delete iopts;
}
