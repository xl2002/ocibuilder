#if !defined(IMAGEBUILDAH_STAGE_EXECUTOR_H)
#define IMAGEBUILDAH_STAGE_EXECUTOR_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <chrono>
#include <memory>
#include <mutex>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "imagebuildah/executor.h"
#include "buildah/buildah.h"
#include "imagebuilder/builder.h"
class Executor;
// class VolumeSet;
// class Copy;
// class Run;
// class Executor_Interface;
class StageExecutor:public Executor_Interface {
public:
    // 成员变量
    // std::shared_ptr<void> ctx; // 假设context类型的实现，以void指针代替。可以换为合适的C++ context类型或自定义实现
    std::shared_ptr<Executor> executor=std::make_shared<Executor>();
    std::function<void(std::string format,std::vector<std::string>args)> log;
    int index=0;
    std::shared_ptr<Stages> stages=std::make_shared<Stages>();
    std::string name;
    std::shared_ptr<Builder> builder=std::make_shared<Builder>();
    int preserved=0;
    std::shared_ptr<VolumeSet> volumes=std::make_shared<VolumeSet>();
    std::map<std::string, std::string> volumeCache;
    std::map<std::string, struct stat> volumeCacheInfo; // Use `struct stat` to hold file info
    std::string mountPoint;
    std::string output;
    std::vector<std::string> containerIDs;
    std::shared_ptr<Stage> stage=std::make_shared<Stage>();
    bool didExecute=false;
    std::vector<std::string> argsFromContainerfile;

    // 构造函数
    StageExecutor()=default;
    void Delete();
    std::tuple<std::string,std::shared_ptr<Canonical_interface>,bool> Execute(std::string base);
    std::shared_ptr<Builder> prepare(
        std::string from,
        bool initializeIBConfig,bool rebase,bool preserveBaseImageAnnotations,
        std::shared_ptr<PullPolicy> pullPolicy
    );
    void UnrecognizedInstruction(std::shared_ptr<Step> step) override;
    void Preserve(std::string path)override;
    void EnsureContainerPath(std::string path)override;
    void EnsureContainerPathAs(std::string path,std::string user,const mode_t* mode) override;
    void COPY(std::vector<std::string> excludes,std::vector<Copy> copies) override;
    void RUN(std::shared_ptr<Run> run) override;
    std::string getContentSummaryAfterAddingContent();
    std::string getCreatedBy(std::shared_ptr<Node> node,std::string addedContentSummary);
    std::pair<std::string,std::shared_ptr<Canonical_interface>> commit(
        std::string createdBy,
        bool emptyLayer,
        std::string output,
        bool squash,
        bool finalInstruction
    );
    void generateBuildOutput(std::shared_ptr<BuildOutputOption>buildOutputOpts);

};  

#endif // IMAGEBUILDAH_STAGE_EXECUTOR_H)
