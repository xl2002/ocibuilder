#include "imagebuildah/executor.h"
#include "config/config.h"
#include "cobra/error.h"
#include "specs/specs.h"
#include "parse/parse.h"
#include "config/new.h"
#include "go/string.h"
#include "imagebuildah/util.h"
#include "imagebuilder/shell_parser.h"
#include "libimage/normalize.h"
#include "multierror/group.h"
#include "storage/storage_transport.h"
#include "go/file.h"
// #include "docker/container.h"
// newExecutor creates a new instance of the imagebuilder.Executor interface.
std::shared_ptr<Executor> 
newExecutor(
    std::string logPrefix,
    std::shared_ptr<store> stores,
    std::shared_ptr<define_BuildOptions> options,
    std::shared_ptr<Node> mainNode,
    std::vector<std::string> containerFiles){
    std::shared_ptr<Config> defaultContainerConfig;
    try
    {
        defaultContainerConfig=Config_defaut();
    }
    catch(const myerror& e)
    {
        throw myerror("failed to get container config: "+string(e.what()));
    }
    auto excludes=options->Excludes;
    if(excludes.size()==0){
        std::tie(excludes,options->IgnoreFile)=ContainerIgnoreFile(options->ContextDirectory,options->IgnoreFile,containerFiles);
    }
    auto capabilities=defaultContainerConfig->Capabilities("",options->AddCapabilities, options->DropCapabilities);
    std::vector<Mount> transientmounts;
    auto allVolumes = defaultContainerConfig->Volumes();  // 获取默认的容器配置卷
    auto transient = options->TransientMounts;     // 获取临时挂载点
    // 将 transientMounts 中的元素追加到 allVolumes 中
    allVolumes.insert(allVolumes.end(), transient.begin(), transient.end());
    // 遍历 allVolumes 中的每一个 volume
    for (const auto& volume : allVolumes) {
        // 在这里对 volume 执行相应操作
    }
    auto secrets=Secrets(options->CommonBuildOpts->Secrets);
    auto writer=options->ReportWriter;
    if(options->Quiet){
        writer=nullptr;
    }
    std::ostream* rusageLogFile=nullptr;
    if(options->LogRusage && !options->Quiet){

    }
    auto exec=std::make_shared<Executor>();

    exec->args=options->Args;
    exec->cacheFrom=options->CacheFrom;
    exec->cacheTo=options->CacheTo;
    exec->cacheTTL=options->CacheTTL;
    exec->containerSuffix=options->ContainerSuffix;
    exec->stages=std::map<std::string, std::shared_ptr<StageExecutor>>();
    exec->store=stores;
    exec->contextDir=options->ContextDirectory;
    exec->excludes=excludes;
    exec->groupAdd=options->GroupAdd;
    exec->ignoreFile=options->IgnoreFile;
    exec->pullPolicy=options->PullPolicy;
    exec->registry=options->Registry;
    exec->ignoreUnrecognizedInstructions=options->IgnoreUnrecognizedInstructions;
    exec->quiet=options->Quiet;
    exec->runtime=options->Runtime;
    exec->runtimeArgs=options->RuntimeArgs;
    exec->transientMounts=transientmounts;
    exec->compression=options->Compression;
    exec->output=options->Output;
    exec->outputFormat=options->OutputFormat;
    exec->additionalTags=options->AdditionalTags;
    exec->signaturePolicyPath=options->SignaturePolicyPath;
    exec->skipUnusedStages=options->SkipUnusedStages;
    exec->systemContext=options->systemContext;
    exec->log=options->Log;
    exec->in=options->In;
    exec->out=options->Out;
    exec->err=options->Err;
    exec->reportWriter=writer;
    exec->isolation=options->Isolation;
    exec->namespaceOptions=options->Namespaceoptions;
    exec->configureNetwork=options->ConfigureNetwork;
    exec->cniPluginPath=options->CNIPluginPath;
    // exec->networkInterface=options->NetworkInterface;
    exec->idmappingOptions=options->IDMappingoptions;
    exec->commonBuildOptions=options->CommonBuildOpts;
    exec->defaultMountsFilePath=options->DefaultMountsFilePath;
    exec->iidfile=options->IIDFile;
    exec->squash=options->Squash;
    exec->labels=options->Labels;
    exec->layerLabels=options->LayerLabels;
    exec->annotations=options->Annotations;
    exec->layers=options->Layers;
    exec->noHostname=options->CommonBuildOpts->NoHostname;
    exec->noHosts=options->CommonBuildOpts->NoHosts;
    exec->useCache=!options->NoCache;
    exec->removeIntermediateCtrs=options->RemoveIntermediateCtrs;
    exec->forceRmIntermediateCtrs=options->ForceRmIntermediateCtrs;
    exec->imageMap=std::map<std::string,std::string>();
    exec->containerMap=std::map<std::string,std::shared_ptr<Builder>>();
    exec->baseMap=std::set<std::string>();
    exec->rootfsMap=std::set<std::string>();
    exec->blobDirectory=options->BlobDirectory;
    exec->unusedArgs=std::set<std::string>();
    exec->capabilities=capabilities;
    exec->deviceSpecs=options->Devices;
    exec->signBy=options->SignBy;
    exec->architecture=options->Architecture;
    exec->timestamp=options->Timestamp;
    exec->os=options->OS;
    exec->maxPullPushRetries=options->MaxPullPushRetries;
    exec->retryPullPushDelay=options->PullPushRetryDelay;
    exec->ociDecryptConfig=options->OciDecryptConfig;
    exec->terminatedStage=std::map<std::string,std::string>();
    exec->stagesSemaphore=options->JobSemaphore;
    exec->logRusage=options->LogRusage;
    exec->rusageLogFile=rusageLogFile;
    exec->imageInfoCache=std::map<std::string, imageTypeAndHistoryAndDiffIDs>();
    exec->fromOverride=options->From;
    exec->additionalBuildContexts=options->AdditionalBuildContexts;
    exec->manifest=options->Manifest;
    exec->secrets=secrets;
    // exec->sshsources=sshsources;
    exec->logPrefix=logPrefix;
    exec->unsetEnvs=options->UnsetEnvs;
    exec->unsetLabels=options->UnsetLabels;
    exec->buildOutput=options->BuildOutput;
    exec->osVersion=options->OSVersion;
    exec->osFeatures=options->OSFeatures;
    exec->envs=options->Envs;
    exec->confidentialWorkload=options->ConfidentialWorkload;
    exec->sbomScanOptions=options->SBOMScanOptions;
    exec->cdiConfigDir=options->CDIConfigDir;

    auto foundFirstStage=false;
    auto globalArgs=std::map<string,string>();
    for (auto& line: mainNode->Children) {
        auto node=line;
        while(node!=nullptr) {
            if(toUpper(node->Value)=="ARG"){

            }
            if(toUpper(node->Value)=="FROM"){
                foundFirstStage=true;
                break;
            }
            // node=node->Next;
            break;
        }
    }
    exec->globalArgs=globalArgs;
    return exec;
}

// Build takes care of the details of running Prepare/Execute/Commit/Delete
// over each of the one or more parsed Dockerfiles and stages.
std::tuple<string,std::shared_ptr<canonical>> Executor::Build(std::shared_ptr<Stages> stages){
    std::string imageID;
    if(stages->Stages.size()==0){
        throw myerror("building: no stages to build");
    }
    std::vector<std::string> cleanupImages;
    std::map<int, std::shared_ptr<StageExecutor>> cleanupStages;

    auto Stdout = out;
    if (quiet) {
        out = new std::ostringstream(); // 类似于io.Discard
    }

    // 清理函数，用于清理阶段容器、镜像等资源
    auto cleanup = [this, &cleanupStages, &cleanupImages,&imageID]() {
        std::exception_ptr lastErr = nullptr;
        // 锁住stagesLock，清理所有成功的阶段容器
        std::unique_lock<std::mutex> lock(this->stagesLock);
        for (const auto& stage : cleanupStages) {
            try {
                stage.second->Delete();
            } catch (const myerror& e) {
                // logrus::Debug("Failed to cleanup stage containers: ", e.what());
                throw e;
            }
        }
        cleanupStages.clear();
        lock.unlock();
        // 清理containerMap中的构建器
        for (const auto& builder : this->containerMap) {
            try {
                builder.second->Delete();
            } catch (const myerror& e) {
                // logrus::Debug("Failed to cleanup image containers: ", e.what());
                // lastErr = std::current_exception();
                throw e;
            }
        }
        this->containerMap.clear();
        // 如果需要删除中间容器，则进行清理
        if (this->removeIntermediateCtrs) {
            // try {
            //     this->deleteSuccessfulIntermediateCtrs();
            // } catch (const myerror& e) {
            //     // logrus::Debug("Failed to cleanup intermediate containers: ", e.what());
            //     // lastErr = std::current_exception();
            //     throw;
            // }
        }
        // 清理中间镜像
        for (size_t i = 0; i < cleanupImages.size(); ++i) {
            const auto& removeID = cleanupImages[cleanupImages.size() - i - 1];
            if (removeID == imageID) continue;
            try {
                // this->store->DeleteImage(removeID, true);
            } catch (const myerror& e) {
                // logrus::Debug("failed to remove intermediate image: ", e.what());
                // if (this->forceRmIntermediateCtrs) {
                //     throw;
                // }
                throw;
            }
        }
        cleanupImages.clear();
        
    };

    std::map<std::string, std::shared_ptr<stageDependencyInfo>> dependencyMap;

    for (size_t stageIndex = 0; stageIndex < stages->Stages.size(); ++stageIndex) {
        const auto& stage = stages->Stages[stageIndex];
        dependencyMap[stage.Name] = std::make_shared<stageDependencyInfo>(stage.Name, stage.Position);

        auto node = stage.Node;
        while (node != nullptr) {
            for (const auto& child : node->Children) {
                if (child->Value == "FROM") {
                    if (child->Next) {
                        if(fromOverride!=""){
                            child->Next->Value=fromOverride;
                            fromOverride="";
                        }
                        std::string base = child->Next->Value;
                        if (!base.empty() && base != "scratch") {
                            if (additionalBuildContexts.find(child->Next->Value) != additionalBuildContexts.end()) {
                                if (additionalBuildContexts[child->Next->Value]->IsImage) {
                                    child->Next->Value = additionalBuildContexts[child->Next->Value]->Value;
                                    base = child->Next->Value;
                                }
                            }
                            auto headingArgs = argsMapToSlice(stage.image_builder->HeadingArgs);
                            auto userArgs=argsMapToSlice(stage.image_builder->Args);
                            userArgs.insert(userArgs.end(), headingArgs.begin(), headingArgs.end());
                            std::string baseWithArg;
                            try{
                                baseWithArg = ProcessWord(base, userArgs);
                            }catch(const myerror& e){
                                throw myerror("while replacing arg variables with values for format "+base+" : "+std::string(e.what())+" in stage ");
                            }
                            baseMap.emplace(baseWithArg);
                            if (additionalBuildContexts.find(baseWithArg) == additionalBuildContexts.end()) {
                                if (dependencyMap.find(baseWithArg) != dependencyMap.end()) {
                                    dependencyMap[stage.Name]->Needs.push_back(baseWithArg);
                                }
                            }
                        }
                    }
                }
                // 处理 ADD 和 COPY 指令
                else if (child->Value == "ADD" || child->Value == "COPY") {
                    for (const auto& flag : child->Flags) {
                        if (flag.find("--from=") == 0) {
                            std::string rootfs = flag.substr(7);
                            rootfsMap.emplace(rootfs);
                            std::string stageName = rootfs;

                            std::vector<std::string> headingArgs = argsMapToSlice(stage.image_builder->HeadingArgs);
                            auto userArgs=argsMapToSlice(stage.image_builder->Args);

                            userArgs.insert(userArgs.end(), headingArgs.begin(), headingArgs.end());

                            auto baseWithArg = ProcessWord(stageName, userArgs);
                            stageName = baseWithArg;

                            int index;
                            if (std::istringstream(stageName) >> index) {
                                stageName = stages->Stages[index].Name;
                            }

                            if (additionalBuildContexts.find(stageName) == additionalBuildContexts.end()) {
                                if (dependencyMap.find(stageName) != dependencyMap.end()) {
                                    dependencyMap[stage.Name]->Needs.push_back(stageName);
                                }
                            }
                        }
                    }
                }
                else if(child->Value=="RUN"){
                    for(const auto& flag:child->Flags){
                        if(flag.find("--mount=")==0&& flag.find("from")!=std::string::npos){
                            std::string mountFlags  = flag.substr(9);
                            std::vector<std::string> fields = split(mountFlags, ',');
                            for (const auto& field : fields) {
                                std::string mountFrom;
                                if (cutPrefix(field, "from=", mountFrom)) {
                                    // Check if this base is a stage; if yes, add base to current stage's dependency tree
                                    // but also confirm if this is not in additional context.
                                    if (additionalBuildContexts.find(mountFrom) == additionalBuildContexts.end()) {
                                        // Treat from as a rootfs we need to preserve
                                        rootfsMap.emplace(mountFrom);
                                        auto it = dependencyMap.find(mountFrom);
                                        if (it != dependencyMap.end()) {
                                            // Update current stage's dependency info
                                            auto currentStageInfo = dependencyMap[stage.Name];
                                            currentStageInfo->Needs.push_back(mountFrom);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            node = node->Next;
        }
        if (stage.Position == (stages->Stages.size() - 1)) {
            markDependencyStagesForTarget(dependencyMap, stage.Name);
        }
    }
    warnOnUnsetBuildArgs(stages->Stages, dependencyMap, args);

    struct Result {
        int Index;
        std::string ImageID;
        bool OnlyBaseImage;
        std::shared_ptr<canonical> Ref=nullptr;
        std::string Error;
        Result() = default;
        Result(int index, std::string imageID, bool onlyBaseImage, std::shared_ptr<canonical> ref, std::string error) 
        : Index(index), ImageID(imageID), OnlyBaseImage(onlyBaseImage), Ref(ref), Error(error) {}
    };

    std::list<Result> results;

    if (!stagesSemaphore) {
        // stagesSemaphore = std::make_shared<semaphore::Semaphore>(stages.size());
    }
    // WaitGroup wg;
    // wg.Add(stages->Stages.size());
    // std::future<Result> cleanupStages = std::async(std::launch::async, [&]() ->Result {
    //     auto cancel=false;
    //     for (size_t stageIndex = 0; stageIndex < stages->Stages.size(); ++stageIndex) {
    //         auto Index = stageIndex;
    //         if (!stagesSemaphore->Acquire(1)) {
    //             cancel = true;
    //             Result res;
    //             res.Index = Index;
    //             res.Error = "build canceled";
    //             wg.Done();
    //             return res;
    //         }
    //         {
    //             std::lock_guard<std::mutex> lock(stagesLock);
    //             if (!cleanupStages.empty()) {;
    //                 return res;
    //             }
    //         }
    //         futures.push_back(std::async(std::launch::async, [this, stageIndex, &cleanupStages, &stages, &dependencyMap, &wg]() -> Result {
    //             Result res;
    //             {
    //                 std::lock_guard<std::mutex> lock(stagesLock);
    //                 if (!cleanupStages.empty()) {;
    //                     return res;
    //                 }
    //             }
    //             if (dependencyMap[stages[stageIndex].Name]->NeededByTarget || skipUnusedStages != types::OptionalBoolFalse) {
    //                 res.Error = std::make_exception_ptr(std::runtime_error("not building stage"));
    //                 return res;
    //             }
    //             auto stageID = buildStage(ctx, cleanupStages, stages, stageIndex);
    //             if (stageID.second) {
    //                 res.Error = std::make_exception_ptr(std::runtime_error("build stage failed"));
    //             } else {
    //                 res.ImageID = stageID.first;
    //                 res.Ref = stageID.second;
    //             }
    //             return res;
    //         }));
    //         wg.Done();
    //     }
    // });
    boost::thread_group threads;
    std::atomic<bool> cancel(false);
    std::mutex resultMutex;
    // boost::optional<std::vector<int>> cleanupStages;

    for (size_t stageIndex = 0; stageIndex < stages->Stages.size(); ++stageIndex) {
        int index = stageIndex;
        threads.create_thread([&, index] {
            try {
                stagesSemaphore->Acquire( 1);  // 获取信号量
            } catch (const myerror& e) {
                cancel = true;
                std::lock_guard<std::mutex> lock(resultMutex);
                results.push_back(Result{index, "",false, nullptr, std::string(e.what())});
                return;
            }

            {
                std::lock_guard<std::mutex> lock(stagesLock);
                auto cleanupstages = cleanupStages;
            }

            // 创建新线程执行阶段构建
            threads.create_thread([&, index] {
                // try {
                    
                    if (cancel || cleanupStages.empty()) {
                        std::lock_guard<std::mutex> lock(resultMutex);
                        std::string stageName = std::to_string(index);

                        std::string err = "not building stage " + std::to_string(index) + ": build canceled";
                        results.push_back(Result{index, "", false, nullptr, err});
                        return;
                    }

                    // 模拟构建阶段
                    std::string stageID,stageErr;
                    std::shared_ptr<canonical> stageRef;
                    bool stageOnlyBaseImage;
                    std::tie (stageID, stageRef, stageOnlyBaseImage, stageErr) = buildStage(cleanupStages, stages, index);

                    if (stageErr!="") {
                        cancel = true;
                        std::lock_guard<std::mutex> lock(resultMutex);
                        results.push_back(Result{index, "", stageOnlyBaseImage,nullptr, stageErr});
                        return;
                    }

                    std::lock_guard<std::mutex> lock(resultMutex);
                    results.push_back(Result{index, stageID, stageOnlyBaseImage,stageRef, std::string("")});
                    stagesSemaphore->Release(1);  // 释放信号量
                // } catch (const myerror& e) {
                //     // cancel = true;
                //     // auto lastError = std::string(e.what());
                //     // std::lock_guard<std::mutex> lock(resultMutex);
                //     // results.push_back(Result{index, stageID, stageRef, stageOnlyBaseImage, lastError});
                //     throw;
                // }
            });
        });
    }
    threads.join_all();  // 等待所有线程完成
    
    std::shared_ptr<canonical> ref;
    for (auto& r : results) {
        auto stage=stages->Stages[r.Index];
        std::unique_lock<std::mutex> lock(stagesLock);
        terminatedStage[stage.Name] = r.Error;
        terminatedStage[to_string(r.Index)] = r.Error;
        if (!r.Error.empty()) {

        }
        if(r.Index<stages->Stages.size()-1 && r.ImageID!=""){
            imageMap[stage.Name] = r.ImageID;
            if(layers && r.OnlyBaseImage){
                cleanupImages.push_back(r.ImageID);
            }
        }
        if(r.Index==stages->Stages.size()-1){
            imageID = r.ImageID;
            ref = r.Ref;
        }
        lock.unlock();
    }

    if (!unusedArgs.empty()) {
        std::vector<std::string> unusedList(unusedArgs.begin(), unusedArgs.end());
        std::sort(unusedList.begin(), unusedList.end());
        // logrus::Warnf("build: unused build args: %s", joinStrings(unusedList, ", ").c_str());
    }
    std::shared_ptr<ImageReference> dest;
    try{
        dest=resolveNameToImageRef(output);
        if(dest->Transport()->Name()==Transport->Name()){
            
        }
    }catch(const myerror& e){
        throw;
    }
    try{
        cleanup();
    }catch(const myerror& e){
        throw;
    }
    if(iidfile!=""){
        try{
            WriteFile(iidfile,"sha256:"+imageID);
        }catch(const myerror& e){
            throw myerror("failed to write image ID file "+iidfile+" : "+string(e.what()));
        }
    }else{
        try{
            *Stdout<<imageID<<std::endl;
        }catch(const myerror& e){
            throw myerror("failed to write image ID to stdout : "+string(e.what()));
        }
        
    }
    return std::make_tuple(imageID, ref);
    // return std::make_tuple("",std::shared_ptr<canonical>(nullptr));
}

std::tuple<std::string,std::shared_ptr<canonical>,bool,std::string> Executor::buildStage(
    std::map<int,std::shared_ptr<StageExecutor>> cleanupStages,
    std::shared_ptr<Stages> stages,
    int stageIndex
){

    return std::make_tuple("",std::shared_ptr<canonical>(nullptr),false,"");
    // StageExecutor* stage = stages[stageIndex];
    // std::string base;

    // try {
    //     base = stage->from();  // 假设 `from()` 方法存在
    // } catch (const std::exception& e) {
    //     // 打印调试信息并返回错误
    //     std::cerr << "buildStage: Error while calling from() " << e.what() << std::endl;
    //     return std::make_tuple("", "", false, std::make_exception_ptr(e));
    // }

    // std::string output = "";
    // if (stageIndex == stages.size() - 1) {
    //     output = this->output;
    //     if (!labels.empty()) {
    //         std::string labelLine;
    //         for (const auto& labelSpec : labels) {
    //             size_t pos = labelSpec.find('=');
    //             if (pos != std::string::npos) {
    //                 std::string key = labelSpec.substr(0, pos);
    //                 std::string value = labelSpec.substr(pos + 1);
    //                 if (!key.empty()) {
    //                     labelLine += " \"" + key + "\"=\"" + value + "\"";
    //                 }
    //             }
    //         }
    //         if (!labelLine.empty()) {
    //             // 假设 ParseDockerfile 方法存在
    //             auto additionalNode = stage->parseDockerfile("LABEL " + labelLine + "\n");
    //             stage->appendNode(additionalNode);
    //         }
    //     }
    // }

    // if (!envs.empty()) {
    //     std::string envLine;
    //     for (const auto& envSpec : envs) {
    //         size_t pos = envSpec.find('=');
    //         if (pos != std::string::npos) {
    //             std::string key = envSpec.substr(0, pos);
    //             std::string value = envSpec.substr(pos + 1);
    //             if (!key.empty()) {
    //                 envLine += " \"" + key + "\"=\"" + value + "\"";
    //             }
    //         } else {
    //             return std::make_tuple("", "", false, std::make_exception_ptr(std::runtime_error("BUG: unresolved environment variable: " + envSpec)));
    //         }
    //     }
    //     if (!envLine.empty()) {
    //         auto additionalNode = stage->parseDockerfile("ENV " + envLine + "\n");
    //         stage->prependNode(additionalNode);
    //     }
    // }

    // std::lock_guard<std::mutex> lock(this->stagesLock);
    // StageExecutor* stageExecutor = stage->startStage(output);

    // if (!stageExecutor->log) {
    //     int stepCounter = 0;
    //     stageExecutor->log = [this, &stepCounter, stageIndex, stages](const std::string& format, const std::vector<std::string>& args) {
    //         std::string prefix = this->logPrefix;
    //         if (stages.size() > 1) {
    //             prefix += "[" + std::to_string(stageIndex + 1) + "/" + std::to_string(stages.size()) + "] ";
    //         }
    //         if (format.find("COMMIT") == std::string::npos) {
    //             stepCounter++;
    //             prefix += "STEP " + std::to_string(stepCounter) + "/" + std::to_string(stages[stageIndex]->getChildrenCount() + 1) + ": ";
    //         }
    //         std::string message = prefix + format + "\n";
    //         std::printf(message.c_str(), args);
    //     };
    // }

    // if (forceRmIntermediateCtrs || !layers) {
    //     cleanupStages[stage->getPosition()] = stageExecutor;
    // }

    // std::string imageID, ref;
    // bool onlyBaseImage;
    // try {
    //     std::tie(imageID, ref, onlyBaseImage) = stageExecutor->execute(base);
    // } catch (const std::exception& e) {
    //     return std::make_tuple("", "", onlyBaseImage, std::make_exception_ptr(e));
    // }

    // if (removeIntermediateCtrs && stage->hasChildren()) {
    //     cleanupStages[stage->getPosition()] = stageExecutor;
    // }

    // return std::make_tuple(imageID, ref, onlyBaseImage, nullptr);
}
void markDependencyStagesForTarget(
    std::map<std::string, std::shared_ptr<stageDependencyInfo>>& dependencyMap,
    const std::string& stage
) {
    auto it = dependencyMap.find(stage);
    if (it != dependencyMap.end()) {
        std::shared_ptr<stageDependencyInfo> stagedependencyInfo = it->second;
        if (!stagedependencyInfo->NeededByTarget) {
            stagedependencyInfo->NeededByTarget = true;
            for (const auto& need : stagedependencyInfo->Needs) {
                markDependencyStagesForTarget(dependencyMap, need);
            }
        }
    }
}

void Executor::warnOnUnsetBuildArgs(
    const std::vector<Stage>& stages,
    const std::map<std::string, std::shared_ptr<stageDependencyInfo>>& dependencyMap,
    const std::map<std::string, std::string>& args
) {
    std::set<std::string> argFound;

    for (const auto& stage : stages) {
        auto node = stage.Node; // first line
        while (node != nullptr) { // each line
            for (const auto& child : node->Children) {
                std::string valueUpper = toUpper(child->Value);
                if (valueUpper == "ARG") {
                    std::string argName = child->Next->Value;
                    size_t equalPos = argName.find('=');
                    if (equalPos != std::string::npos) {
                        std::string argValue = argName.substr(equalPos + 1);
                        if (!argValue.empty()) {
                            argFound.insert(argName.substr(0, equalPos));
                        }
                    }
                    bool argHasValue = true;
                    if (equalPos == std::string::npos) {
                        argHasValue = argFound.count(argName) > 0;
                    }
                    if (args.find(argName) == args.end() && !argHasValue) {
                        bool shouldWarn = true;
                        auto it = dependencyMap.find(stage.Name);
                        if (it != dependencyMap.end()) {
                            const std::shared_ptr<stageDependencyInfo> stageDependencyInfo = it->second;
                            if (!stageDependencyInfo->NeededByTarget && *skipUnusedStages != OptionalBool::OptionalBoolFalse) {
                                shouldWarn = false;
                            }
                        }
                        // if (builtinAllowedBuildArgs.find(argName) != builtinAllowedBuildArgs.end()) {
                        //     shouldWarn = false;
                        // }
                        if (globalArgs.find(argName) != globalArgs.end()) {
                            shouldWarn = false;
                        }
                        // if (shouldWarn) {
                        //     logger.Warnf("missing %q build argument. Try adding %q to the command line",
                        //                     argName,
                        //                     fmt::format("--build-arg {}=<VALUE>", argName));
                        // }
                    }
                }else{
                    continue;
                }
            }
            node = node->Next;
        }
    }
}

// 解析输出名称并返回对应的 ImageReference
std::shared_ptr<ImageReference> Executor::resolveNameToImageRef(const std::string& output) {
    try {
        // 尝试解析 ImageName
        auto imageRef = ParseImageName(output);
        return imageRef;
    } catch (const myerror& e) {
        // 解析失败，尝试规范化名称
        auto resolved = NormalizeName(output);
        if (!resolved) {
            return nullptr;
        }
        try {
            // 尝试解析 StoreReference
            auto imageRef = Transport->ParseStoreReference(store, resolved->String());
            return imageRef;
        } catch (const myerror& e) {
            // 解析失败，返回空和异常
            return nullptr;
        }
    }

}
