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
#include "network/network.h"
#include "utils/cli/cli/common.h"
#include "security/auth.h"
#include "utils/common/go/string.h"
#include "utils/common/go/file.h"
/**
 * @brief 初始化 push 命令的内容
 * 
 */
void init_push(){
    auto opts=std::make_shared<pushOptions>() ;
    string name{"push"};
    string Short{"Push an image to a specified destination"};
    string Long{"Pushes an image to a specified location."};
    string example{"buildah pull imagename\n  buildah pull docker-daemon:imagename:imagetag\n  buildah pull myregistry/myrepository/imagename:imagetag"};
    Command* pushCommand=new Command{name,Short,Long,example};
    string Template=UsageTemplate();
    pushCommand->SetUsageTemplate(Template);

    Flagset* flags=pushCommand->Flags();
    // flags.StringVar();
    flags->SetInterspersed(false);
    flags->StringVar(opts->format, "format", std::string(), "manifest type (oci, v2s1, or v2s2) to use in the destination (default is manifest type of source, with fallbacks)");
    flags->StringVar(opts->authfile, "authfile", std::string(), "path of the authentication file. Use REGISTRY_AUTH_FILE environment variable to override");
    pushCommand->Run=[=](Command& cmd, vector<string> args){
        pushCmd(cmd,args,opts);
    };
    rootcmd.AddCommand({pushCommand});
    // return imagesCommand;
}

/**
 * @brief push 命令Run操作的
 * 
 */
void pushCmd(Command& cmd, vector<string> args,std::shared_ptr<pushOptions> iopts){
    //执行流程
    std::string src,destSpec;
    //1. 获得令牌
    CheckAuthFile(iopts);
    if(args.size()==0){
        std::cout<<"Please input the image you want to push!!"<<"\n";
        return;
    }else if(args.size()==1){
        src=args[0];
        destSpec=src;
    }else if(args.size()==2){
        src=args[0];
        destSpec=args[1];
    }else{
        std::cerr<<"the number of arguments is not right!!"<<"\n";
    }
    auto compress=compression::Gzip;
    //读取本地镜像的数据
    auto store=getStore(&cmd);
    
    //分析镜像名
    std::string urlpath, withinTransport;
    std::tie(urlpath, withinTransport, std::ignore) = Cut(src, '/');
    std::tie(std::ignore,withinTransport,std::ignore)=Cut(withinTransport,'/');
    auto imagestore=store->Image(withinTransport);

    //拿到push命令中的这一部分

    // std::string image="localhost:5000/busybox:latest";
    //解析出来host、port、镜像名等
    dockerClient client;
    auto url=client.resolveRequestURL(src);

    //  执行登录请求
    login("admin","Harbor12345",url->host,url->port);

    //url->host  url->port  url->imageName
    if(!ifSupportV2(url->host,url->port)){
        std::cout<<"Can't push!!"<<"\n";
    }
    //读取本地镜像层的数据
    //先上传blobs数据层
    auto layers=imagestore->image_manifest->Layers;
    auto blobsNum=layers.size();
    //依次上传每层blob
    std::string uid;
    std::string state;
    for(int i=0;i<blobsNum;i++){
        //拿到每层数据data的路径还有hash值
        std::string shaId=layers[i].Digests.Encoded();
        std::string filePath=store->image_store_dir+"/blobs/sha256/"+shaId;
        if(!isCorrect(shaId,filePath)){
            std::cerr<<"the blob: "+shaId+" is not correct!!"<<std::endl;
        }
        std::string fisrtTwoC=shaId.substr(0, 2);
        //判断这层数据是否在服务器存在，不存在再传输
        if(!ifBlobExists(url->host,url->port,url->imageName,shaId,url->projectName)){
            std::pair<std::string, std::string> initResult = initUpload(url->host, url->port, url->imageName,url->projectName);
            uid = initResult.first;
            state = initResult.second;
            //拿到data数据大小
            std::ifstream file(filePath, std::ios::binary | std::ios::ate);
            std::size_t total_size = file.tellg();
            file.close();
            //上传数据
            state = uploadBlobChunk(url->host, url->port,uid,state,filePath,0,total_size,total_size, url->imageName,url->projectName);
            //完成本次上传
            finalizeUpload(url->host, url->port,uid,shaId,state,url->imageName,url->projectName);
        }
    }

    //再上传config数据
    std::string shaId1=imagestore->image_manifest->Config.Digests.Encoded();
    std::string configPath=store->image_store_dir+"/blobs/sha256/"+shaId1;
    if(!isCorrect(shaId1,configPath)){
        std::cerr<<"the config: "+shaId1+" is not correct!!"<<std::endl;
    }
    std::string fisrtTwoC=shaId1.substr(0, 2);
    //判断这层数据是否在服务器存在，不存在再传输
    if(!ifBlobExists(url->host,url->port,url->imageName,shaId1,url->projectName)){
        std::pair<std::string, std::string> initResult = initUpload(url->host, url->port, url->imageName,url->projectName);
        uid = initResult.first;
        state = initResult.second;

        //拿到data数据大小
        std::ifstream file(configPath, std::ios::binary | std::ios::ate);
        std::size_t total_size = file.tellg();
        file.close();
        //上传数据
        state = uploadBlobChunk(url->host, url->port,uid,state,configPath,0,total_size,total_size, url->imageName,url->projectName);
        //完成本次上传
        finalizeUpload(url->host, url->port,uid,shaId1,state,url->imageName,url->projectName);
    }

    //最后上传manifest数据
    std::string shaId2=imagestore->digest->Encoded();
    std::string manifestPath=store->image_store_dir+"/blobs/sha256/"+shaId2;
    if(!isCorrect(shaId2,manifestPath)){
        std::cerr<<"the manifest: "+shaId1+" is not correct!!"<<std::endl;
    }
    std::string manifestType=imagestore->image_index->mediaType;
    std::string fisrtTwoC2=shaId2.substr(0, 2);
    //判断这层数据是否在服务器存在，不存在再传输
    if(!ifBlobExists(url->host,url->port,url->imageName,shaId2,url->projectName)){
        // std::pair<std::string, std::string> initResult = initUpload(url->host, url->port, url->imageName);
        // std::string uid = initResult.first;
        // std::string state = initResult.second;

        //拿到data数据大小
        std::ifstream file(manifestPath, std::ios::binary | std::ios::ate);
        std::size_t total_size = file.tellg();
        file.close();
        //上传数据
        uploadManifest(url->host, url->port,manifestPath,0,total_size,url->imageName, url->version,manifestType,url->projectName);
    }

    std::cout<<"Push success!!"<<std::endl;
}