#include "imagebuildah/executor.h"
#include "config/config.h"
#include "cobra/error.h"
#include "specs/specs.h"
#include "parse/parse.h"
#include "config/new.h"
#include "go/string.h"
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
    if(stages->Stages.size()==0){
        throw myerror("building: no stages to build");
    }
    std::vector<std::string> cleanupImages;
    std::map<int, std::shared_ptr<StageExecutor>> cleanupStages;

    auto Stdout = out;
    if (quiet) {
        out = &std::ostringstream(); // 类似于io.Discard
    }

    auto cleanup = [this, &cleanupImages, &cleanupStages]() {
        // std::exception_ptr lastErr;

        // 清理阶段容器
        {
            std::lock_guard<std::mutex> lock(b.stagesLock);
            for (const auto& stage : cleanupStages) {
                try {
                    stage.second->Delete();
                } catch (const std::exception& e) {
                    logrus::Debugf("Failed to cleanup stage containers: %s", e.what());
                    lastErr = std::current_exception();
                }
            }
            cleanupStages.clear();
        }

        // 清理使用的构建器
        for (const auto& builder : b.containerMap) {
            try {
                builder.second->Delete();
            } catch (const std::exception& e) {
                logrus::Debugf("Failed to cleanup image containers: %s", e.what());
                lastErr = std::current_exception();
            }
        }
        b.containerMap.clear();

        // 清理中间容器
        if (b.removeIntermediateCtrs) {
            try {
                b.deleteSuccessfulIntermediateCtrs();
            } catch (const std::exception& e) {
                logrus::Debugf("Failed to cleanup intermediate containers: %s", e.what());
                lastErr = std::current_exception();
            }
        }

        // 删除阶段产生的镜像
        for (size_t i = 0; i < cleanupImages.size(); ++i) {
            std::string removeID = cleanupImages[cleanupImages.size() - i - 1];
            if (removeID == imageID) {
                continue;
            }
            try {
                b.store->DeleteImage(removeID, true);
            } catch (const std::exception& e) {
                logrus::Debugf("failed to remove intermediate image %s: %s", removeID.c_str(), e.what());
                if (b.forceRmIntermediateCtrs || !dynamic_cast<const storage::ErrImageUsedByContainer*>(&e)) {
                    lastErr = std::current_exception();
                }
            }
        }
        cleanupImages.clear();

        if (b.rusageLogFile && b.rusageLogFile != b.out) {
            try {
                if (auto closer = dynamic_cast<std::ofstream*>(b.rusageLogFile)) {
                    closer->close();
                }
            } catch (...) {
                // 忽略错误
            }
        }
        return lastErr;
    };

    auto cleanupGuard = std::unique_ptr<void, std::function<void(void*)>>(
        nullptr, [&](void*) {
            if (auto cleanupErr = cleanup()) {
                if (err == nullptr) {
                    err = cleanupErr;
                } else {
                    try {
                        std::rethrow_exception(cleanupErr);
                    } catch (const std::exception& e) {
                        err = std::make_exception_ptr(std::runtime_error(e.what()));
                    }
                }
            }
        });

    std::map<std::string, std::shared_ptr<stageDependencyInfo>> dependencyMap;

    for (size_t stageIndex = 0; stageIndex < stages.size(); ++stageIndex) {
        const auto& stage = stages[stageIndex];
        dependencyMap[stage.Name] = std::make_shared<stageDependencyInfo>(stage.Name, stage.Position);

        auto* node = stage.Node;
        while (node != nullptr) {
            for (const auto& child : node->Children) {
                if (child.Value == "FROM") {
                    if (child.Next) {
                        std::string base = child.Next->Value;
                        if (!base.empty() && base != buildah::BaseImageFakeName) {
                            if (b.additionalBuildContexts.find(child.Next->Value) != b.additionalBuildContexts.end()) {
                                if (b.additionalBuildContexts[child.Next->Value].IsImage) {
                                    child.Next->Value = b.additionalBuildContexts[child.Next->Value].Value;
                                    base = child.Next->Value;
                                }
                            }

                            std::vector<std::string> userArgs = argsMapToSlice(stage.Builder.HeadingArgs);
                            userArgs.insert(userArgs.end(), argsMapToSlice(stage.Builder.Args).begin(), argsMapToSlice(stage.Builder.Args).end());

                            auto baseWithArg = imagebuilder::ProcessWord(base, userArgs);
                            b.baseMap[baseWithArg] = {};

                            if (b.additionalBuildContexts.find(baseWithArg) == b.additionalBuildContexts.end()) {
                                if (dependencyMap.find(baseWithArg) != dependencyMap.end()) {
                                    dependencyMap[stage.Name]->Needs.push_back(baseWithArg);
                                }
                            }
                        }
                    }
                }
                // 处理 ADD 和 COPY 指令
                else if (child.Value == "ADD" || child.Value == "COPY") {
                    for (const auto& flag : child.Flags) {
                        if (flag.find("--from=") == 0) {
                            std::string rootfs = flag.substr(7);
                            b.rootfsMap[rootfs] = {};
                            std::string stageName = rootfs;

                            std::vector<std::string> userArgs = argsMapToSlice(stage.Builder.HeadingArgs);
                            userArgs.insert(userArgs.end(), argsMapToSlice(stage.Builder.Args).begin(), argsMapToSlice(stage.Builder.Args).end());

                            auto baseWithArg = imagebuilder::ProcessWord(stageName, userArgs);
                            stageName = baseWithArg;

                            int index;
                            if (std::istringstream(stageName) >> index) {
                                stageName = stages[index].Name;
                            }

                            if (b.additionalBuildContexts.find(stageName) == b.additionalBuildContexts.end()) {
                                if (dependencyMap.find(stageName) != dependencyMap.end()) {
                                    dependencyMap[stage.Name]->Needs.push_back(stageName);
                                }
                            }
                        }
                    }
                }
            }
            node = node->Next;
        }
        if (stage.Position == (stages.size() - 1)) {
            markDependencyStagesForTarget(dependencyMap, stage.Name);
        }
    }
    b.warnOnUnsetBuildArgs(stages, dependencyMap, b.args);

    struct Result {
        int Index;
        std::string ImageID;
        bool OnlyBaseImage;
        std::shared_ptr<reference::Canonical> Ref;
        std::exception_ptr Error;
    };

    std::vector<std::future<Result>> futures;

    if (!b.stagesSemaphore) {
        b.stagesSemaphore = std::make_shared<semaphore::Semaphore>(stages.size());
    }

    for (size_t stageIndex = 0; stageIndex < stages.size(); ++stageIndex) {
        futures.push_back(std::async(std::launch::async, [this, stageIndex, &cleanupStages, &stages, &dependencyMap]() -> Result {
            Result res;
            res.Index = stageIndex;

            if (b.stagesSemaphore->Acquire(ctx, 1)) {
                return res;
            }

            {
                std::lock_guard<std::mutex> lock(b.stagesLock);
                if (!cleanupStages.empty()) {
                    res.Error = std::make_exception_ptr(std::runtime_error("build canceled"));
                    return res;
                }
            }

            if (dependencyMap[stages[stageIndex].Name]->NeededByTarget || b.skipUnusedStages != types::OptionalBoolFalse) {
                res.Error = std::make_exception_ptr(std::runtime_error("not building stage"));
                return res;
            }

            auto stageID = b.buildStage(ctx, cleanupStages, stages, stageIndex);
            if (stageID.second) {
                res.Error = std::make_exception_ptr(std::runtime_error("build stage failed"));
            } else {
                res.ImageID = stageID.first;
                res.Ref = stageID.second;
            }

            return res;
        }));
    }

    std::string imageID;
    std::shared_ptr<reference::Canonical> ref;
    for (auto& future : futures) {
        Result res = future.get();

        if (res.Error) {
            b.lastError = res.Error;
            std::rethrow_exception(res.Error);
        }

        if (res.Index < stages.size() - 1 && !res.ImageID.empty()) {
            b.imageMap[stages[res.Index].Name] = res.ImageID;
            if (!b.layers && !res.OnlyBaseImage) {
                cleanupImages.push_back(res.ImageID);
            }
        }

        if (res.Index == stages.size() - 1) {
            imageID = res.ImageID;
            ref = res.Ref;
        }
    }

    if (!b.unusedArgs.empty()) {
        std::vector<std::string> unusedList(b.unusedArgs.begin(), b.unusedArgs.end());
        std::sort(unusedList.begin(), unusedList.end());
        logrus::Warnf("build: unused build args: %s", joinStrings(unusedList, ", ").c_str());
    }

    if (b.quiet) {
        b.out = stdout;
    }

    return std::make_tuple(imageID, ref, nullptr);
    // return std::make_tuple("",std::shared_ptr<canonical>(nullptr));
}