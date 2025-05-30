#if !defined(CMD_BUILD_IMAGEBUILDER_BUILDER_H)
#define CMD_BUILD_IMAGEBUILDER_BUILDER_H
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
#include "utils/parse/dockerfileparse.h"
#include "image/image_types/docker/types.h"
#include "image/image_types/docker/image.h"
#include "cmd/build/imagebuilder/evaluator.h"
// #include "cmd/build/imagebuildah/stage_executor.h"
#include <boost/filesystem.hpp>
// #include <boost/system/error_code.hpp>
#include <sys/stat.h>
#include "filesys/systems.h"
class File{
    public:
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
    bool Add( const std::string& path);
    bool Has(std::string path);
    bool Covers(std::string path);
};

class Executor_Interface;
class Image_Builder:public std::enable_shared_from_this<Image_Builder> {
public:
    std::shared_ptr<container_Config> RunConfig=std::make_shared<container_Config>();

    std::vector<std::string> Env;
    std::map<std::string, std::string> Args;
    std::map<std::string, std::string> HeadingArgs;
    std::map<std::string, std::string> UserArgs;
    bool CmdSet = false;
    std::string Author;
    
    std::vector<std::string> GlobalAllowedArgs;
    std::map<std::string, bool> AllowedArgs;
    std::shared_ptr<VolumeSet> Volumes=std::make_shared<VolumeSet>();
    std::vector<std::string> Excludes;

    std::shared_ptr<VolumeSet> PendingVolumes=std::make_shared<VolumeSet>();
    std::vector<::Run> PendingRuns;
    std::vector<Copy> PendingCopies;

    std::vector<std::string> Warnings;
    std::string Platform;


    std::string From(std::shared_ptr<Node> node);
    void extractHeadingArgsFromNode(std::shared_ptr<Node> node);
    std::shared_ptr<Image_Builder> builderForStage(std::vector<std::string> globalArgsList);
    std::shared_ptr<Step> Step_new();
    bool RequiresStart(std::shared_ptr<Node> node);
    std::vector<std::string> Arguments();
    void Run(
        std::shared_ptr<Step> step,
        std::shared_ptr<Executor_Interface> exec,
        bool noRunsRemaining 
    );
    std::shared_ptr<container_Config>Config();
    void FromImage(std::shared_ptr<Docker::Image>image,std::shared_ptr<Node>node);
};

std::vector<std::shared_ptr<Node>> SplitBy(std::shared_ptr<Node>node,std::string value);
std::tuple<std::string,bool> extractNameFromNode(std::shared_ptr<Node>node);
std::vector<std::shared_ptr<Node>> SplitChildren(std::shared_ptr<Node>node,std::string value);
// extern const map<std::string,StepFunc> evaluateTable;
extern std::map<std::string,bool> builtinAllowedBuildArgs;
using StepFunc=std::function<void(std::shared_ptr<Image_Builder>,std::vector<std::string>,std::map<std::string,bool>,std::vector<std::string>,std::string,std::vector<Heredoc>)>;
extern std::map<std::string,StepFunc> evaluateTable;

class Stage {
public:
    // Members
    int Position=0;
    std::string Name;
    std::shared_ptr<Image_Builder> image_builder=std::make_shared<Image_Builder>();  // Use std::shared_ptr for managing dynamic memory
    std::shared_ptr<::Node> Node=std::make_shared<::Node>();
};
class Stages {
public:
    std::vector<Stage> Stages;
};
std::vector<std::string> ParseIgnore(const std::string& path);
std::shared_ptr<Image_Builder>NewBuilder(std::map<std::string,std::string>args);
std::shared_ptr<Image_Builder>newBuilderWithGlobalAllowedArgs(std::map<std::string,std::string>args,std::vector<std::string>globalallowedargs);
std::shared_ptr<Stages>NewStages(std::shared_ptr<Node>node,std::shared_ptr<Image_Builder>b);


class Executor_Interface {
    public:
    virtual void Preserve(std::string path) = 0;
    virtual void EnsureContainerPath(std::string path) = 0;
    virtual void EnsureContainerPathAs(std::string path,std::string user,const mode_t* mode) = 0;
    virtual void COPY(std::vector<std::string> excludes,std::vector<Copy> copies) = 0;
    virtual void RUN(std::shared_ptr<Run> run) = 0;
    virtual void UnrecognizedInstruction(std::shared_ptr<Step> step) = 0;
};
class logExecutor:public Executor_Interface{
    public:
    void UnrecognizedInstruction(std::shared_ptr<Step> step) override;
    void Preserve(std::string path)override;
    void EnsureContainerPath(std::string path)override;
    void EnsureContainerPathAs(std::string path,std::string user,const mode_t* mode) override;
    void COPY(std::vector<std::string> excludes,std::vector<Copy> copies) override;
    void RUN(std::shared_ptr<Run> run) override;
};
class noopExecutor:public Executor_Interface{
    public:
    void UnrecognizedInstruction(std::shared_ptr<Step> step) override;
    void Preserve(std::string path)override;
    void EnsureContainerPath(std::string path)override;
    void EnsureContainerPathAs(std::string path,std::string user,const mode_t* mode) override;
    void COPY(std::vector<std::string> excludes,std::vector<Copy> copies) override;
    void RUN(std::shared_ptr<Run> run) override;
};
#endif // IMAGEBUILDER_BUILDER_H)
