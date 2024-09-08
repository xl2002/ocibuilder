#if !defined(IMAGEBUILDER_BUILDER_H)
#define IMAGEBUILDER_BUILDER_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <chrono>
#include <iostream>
#include <fstream>
// #include <vector>
// #include <string>
#include <tuple>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include "parse/dockerfileparse.h"
#include "docker/container.h"
#include "imagebuilder/evaluator.h"
struct File{
    std::string Name;
    std::string Data;
};
class Copy {
public:
    bool FromFS = false;                // 表示是否从文件系统复制到容器
    std::string From;                   // 如果设置了，则从指定的阶段或镜像复制到容器
    std::vector<std::string> Src;       // 源文件路径列表
    std::string Dest;                   // 目标路径
    bool Download = false;              // 是否需要下载
    std::string Chown;                  // 目标的所有者:组，传递给执行器处理
    std::string Chmod;                  // 目标的权限设置
    std::string Checksum;               // 校验和
    std::vector<File> Files;            // 需要执行器为该指令创建的附加文件
};
class Run {
public:
    bool Shell = false;
    std::vector<std::string> Args;
    std::vector<std::string> Mounts;  // 通过 --mount 标志指定的挂载点
    std::string Network;  // 指定容器运行时的网络模式
    std::vector<File> Files;  // 执行器需要为此指令创建的附加文件
};
class VolumeSet {
public:
    std::vector<std::string> Volumes;
};
class Image_Builder {
public:
    std::shared_ptr<container_Config> RunConfig=nullptr;

    std::vector<std::string> Env;
    std::map<std::string, std::string> Args;
    std::map<std::string, std::string> HeadingArgs;
    std::map<std::string, std::string> UserArgs;
    bool CmdSet = false;
    std::string Author;
    
    std::vector<std::string> GlobalAllowedArgs;
    std::map<std::string, bool> AllowedArgs;
    std::shared_ptr<VolumeSet> Volumes=nullptr;
    std::vector<std::string> Excludes;

    std::shared_ptr<VolumeSet> PendingVolumes=nullptr;
    std::vector<Run> PendingRuns;
    std::vector<Copy> PendingCopies;

    std::vector<std::string> Warnings;
    std::string Platform;


    std::string From(std::shared_ptr<Node> node);
    void extractHeadingArgsFromNode(std::shared_ptr<Node> node);
    std::shared_ptr<Image_Builder> builderForStage(std::vector<std::string> globalArgsList);
    std::shared_ptr<Step> Step_new();
    bool RequiresStart(std::shared_ptr<Node> node);
    std::vector<std::string> Arguments();
};

std::vector<std::shared_ptr<Node>> SplitBy(std::shared_ptr<Node>node,std::string value);
std::tuple<std::string,bool> extractNameFromNode(std::shared_ptr<Node>node);
std::vector<std::shared_ptr<Node>> SplitChildren(std::shared_ptr<Node>node,std::string value);



class Stage {
public:
    // Members
    int Position=0;
    std::string Name;
    std::shared_ptr<Image_Builder> image_builder;  // Use std::shared_ptr for managing dynamic memory
    std::shared_ptr<Node> Node;
};
class Stages {
public:
    std::vector<Stage> Stages;
};
std::vector<std::string> ParseIgnore(const std::string& path);
std::shared_ptr<Image_Builder>NewBuilder(std::map<std::string,std::string>args);
std::shared_ptr<Image_Builder>newBuilderWithGlobalAllowedArgs(std::map<std::string,std::string>args,std::vector<std::string>globalallowedargs);
std::shared_ptr<Stages>NewStages(std::shared_ptr<Node>node,std::shared_ptr<Image_Builder>b);




#endif // IMAGEBUILDER_BUILDER_H)
