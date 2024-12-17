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
/**
 * @brief 初始化 push 命令的内容
 * 
 */
void init_push(){
    pushOptions options;
    string name{"push"};
    string Short{"Push an image to a specified destination"};
    string Long{"Pushes an image to a specified location."};
    string example{"buildah pull imagename\n\
                    buildah pull docker-daemon:imagename:imagetag\n\
                    buildah pull myregistry/myrepository/imagename:imagetag"};
    Command* pushCommand=new Command{name,Short,Long,example};
    // pushCommand.Run=pushCmd;
    Flagset* flags=pushCommand->Flags();
    // flags.StringVar();

    rootcmd.AddCommand({pushCommand});
    // return imagesCommand;
}

/**
 * @brief push 命令Run操作的
 * 
 */
void pushCmd(){
    //执行流程
    //拿到push命令中的这一部分
    std::string image="localhost:5000/busybox:latest";
    //解析出来host、port、镜像名等
    dockerClient client;
    auto url=client.resolveRequestURL(image);
    //url->host  url->port  url->imageName
    if(!ifSupportV2(url->host,url->port)){
        std::cout<<"Can't push!!"<<"\n";
    }
    //读取本地镜像层的数据
    //先上传blobs数据层
    int blobsNum=10;
    //依次上传每层blob
    for(int i=1;i<blobsNum;i++){
        //拿到每层数据data的路径还有hash值
        std::string filePath="ads/dsa/s1/s1424535434rcfds/data";
        std::string shaId="1ds111dasd32dasd4214141dsadsad4";
        std::string fisrtTwoC=shaId.substr(0, 2);

        //判断这层数据是否在服务器存在，不存在再传输
        if(!ifBlobExists(url->host,url->port,url->imageName,shaId)){
             std::pair<std::string, std::string> initResult = initUpload(url->host, url->port, url->imageName);
             std::string uid = initResult.first;
             std::string state = initResult.second;

            //拿到data数据大小
            std::ifstream file(filePath, std::ios::binary | std::ios::ate);
            std::size_t total_size = file.tellg();
            file.close();
            //上传数据
            state = uploadBlobChunk(url->host, url->port,uid,state,filePath,0,total_size,total_size, url->imageName);
            //完成本次上传
            finalizeUpload(url->host, url->port,uid,shaId,state,url->imageName);
        }
    }

    //再上传config数据
    std::string configPath="ads/dsa/s1/s1424535434rcfds/data";
    std::string shaId="1ds111dasd32dasd4214141dsadsad4";
    std::string fisrtTwoC=shaId.substr(0, 2);

    //判断这层数据是否在服务器存在，不存在再传输
    if(!ifBlobExists(url->host,url->port,url->imageName,shaId)){
        std::pair<std::string, std::string> initResult = initUpload(url->host, url->port, url->imageName);
        std::string uid = initResult.first;
        std::string state = initResult.second;

        //拿到data数据大小
        std::ifstream file(configPath, std::ios::binary | std::ios::ate);
        std::size_t total_size = file.tellg();
        file.close();
        //上传数据
        state = uploadBlobChunk(url->host, url->port,uid,state,configPath,0,total_size,total_size, url->imageName);
        //完成本次上传
        finalizeUpload(url->host, url->port,uid,shaId,state,url->imageName);
    }

    //最后上传manifest数据
    std::string manifestPath="ads/dsa/s1/s1424535434rcfds/data";
    std::string shaId="1ds111dasd32dasd4214141dsadsad4";
    std::string fisrtTwoC=shaId.substr(0, 2);

    //判断这层数据是否在服务器存在，不存在再传输
    if(!ifBlobExists(url->host,url->port,url->imageName,shaId)){
        std::pair<std::string, std::string> initResult = initUpload(url->host, url->port, url->imageName);
        std::string uid = initResult.first;
        std::string state = initResult.second;

        //拿到data数据大小
        std::ifstream file(manifestPath, std::ios::binary | std::ios::ate);
        std::size_t total_size = file.tellg();
        file.close();
        //上传数据
        state = uploadBlobChunk(url->host, url->port,uid,state,manifestPath,0,total_size,total_size, url->imageName);
        //完成本次上传
        finalizeUpload(url->host, url->port,uid,shaId,state,url->imageName);
    }

}