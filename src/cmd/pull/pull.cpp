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
#include "network/network.h"
#include "utils/cli/cli/common.h"
#include "security/auth.h"
#include "utils/common/go/string.h"
#include "utils/common/go/file.h"
/**
 * @brief 初始化 pull 命令的内容
 * 
 */
void init_pull(){
    pullOptions options;
    auto opts=std::make_shared<pullOptions>();
    string name{"pull"};
    string Short{"Pull an image from the specified location"};
    string Long{"Pulls an image from a registry and stores it locally.\n\
                An image can be pulled using its tag or digest. If a tag is not\n\
                specified, the image with the 'latest' tag (if it exists) is pulled."};
    string example{"buildah pull imagename\n\
                    buildah pull docker-daemon:imagename:imagetag\n\
                    buildah pull myregistry/myrepository/imagename:imagetag"};
    Command* pullCommand=new Command{name,Short,Long,example};
    string Template=UsageTemplate();
    pullCommand->SetUsageTemplate(Template);
    // pullCommand.Run=pullCmd;
    Flagset* flags=pullCommand->Flags();
    // flags.BoolVar();
    flags->SetInterspersed(false);

    pullCommand->Run=[=](Command& cmd, vector<string> args){
        pullCmd(cmd,args,opts);
    };
    rootcmd.AddCommand({pullCommand});
    // return imagesCommand;
}

/**
 * @brief pull 命令Run操作的
 * 
 */
void pullCmd(Command& cmd, vector<string> args,std::shared_ptr<pullOptions> iopts){
    std::string src;
    if(args.size()==0){
        std::cout<<"Please input the image you want to push!!"<<"\n";
        return;
    }else if(args.size()==1){
        src=args[0];
    }else{
        std::cerr<<"the number of arguments is not right!!"<<"\n";
    }

    dockerClient client;
    auto url=client.resolveRequestURL(src);
    login("admin","Harbor12345",url->host,url->port);

    pullManifestAndBlob(url->host,url->port,url->projectName,url->imageName,url->version);

}
