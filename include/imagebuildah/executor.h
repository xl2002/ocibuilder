#if !defined(IMAGEBUILDAH_EXECUTOR_H)
#define IMAGEBUILDAH_EXECUTOR_H
#include <string>
#include <vector>
#include <map>
#include <set>
#include <ctime>
#include <memory>
#include <mutex>
#include <tuple>
#include <iostream>
#include <future>
#include <fstream>
#include <functional>
#include "reference/reference.h"
#include "storage/storage.h"
#include "define/pull.h"
#include "specs/specs.h"
#include "define/build.h"
#include "define/types.h"
#include "define/namespace.h"
#include "semaphore/semaphore.h"
#include "imagebuildah/stage_executor.h"
#include "define/types_unix.h"
#include "digest/digest.h"
#include "v1/config.h"
#include "sshagent/sshagent.h"
#include "parse/dockerfileparse.h"
#include "buildah/buildah.h"
#include "imagebuilder/builder.h"
#include "types/types.h"
#include "alltransports/alltransports.h"
#include <list>
#include <boost/optional.hpp>
#include <boost/thread/thread.hpp> // 使用 boost::thread
class imageTypeAndHistoryAndDiffIDs {
    public:
    std::string manifestType;
    std::shared_ptr<History> history=std::make_shared<History>();
    std::vector<Digest> diffIDs;
    std::string err;
};
class StageExecutor;
struct stageDependencyInfo;
class Stages;
class Stage;
struct Executor {
    std::vector<named> cacheFrom;
    std::vector<named> cacheTo;
    std::chrono::duration<int> cacheTTL;
    std::string containerSuffix;
    // std::shared_ptr<Logger> logger;
    std::map<std::string, std::shared_ptr<StageExecutor>> stages;
    std::shared_ptr<Store> store=std::make_shared<Store>();
    std::string contextDir;
    std::shared_ptr<PullPolicy> pullPolicy=std::make_shared<::PullPolicy>();
    std::string registry;
    bool ignoreUnrecognizedInstructions=false;
    bool quiet=false;
    std::string runtime;
    std::vector<std::string> runtimeArgs;
    std::vector<Mount> transientMounts;
    std::shared_ptr<Compression> compression=std::make_shared<::Compression>();
    std::string output;
    std::string outputFormat;
    std::vector<std::string> additionalTags;
    std::function<void(string format,vector<string>args)> log; // can be nullptr
    std::istream* in=nullptr;
    std::ostream* out=nullptr;
    std::ostream* err=nullptr;
    std::string signaturePolicyPath;
    OptionalBool skipUnusedStages;
    std::shared_ptr<SystemContext> systemContext=std::make_shared<::SystemContext>();
    std::ostream* reportWriter=nullptr;
    std::shared_ptr<Isolation> isolation=std::make_shared<::Isolation>();
    std::vector<NamespaceOption> namespaceOptions;
    std::shared_ptr<NetworkConfigurationPolicy> configureNetwork=std::make_shared<::NetworkConfigurationPolicy>();
    std::string cniPluginPath;
    std::string cniConfigDir;
    // NetworkInterface is the libnetwork network interface used to setup CNI or netavark networks.
    // ContainerNetwork networkInterface;
    std::shared_ptr<IDMappingOptions> idmappingOptions=std::make_shared<::IDMappingOptions>();
    std::shared_ptr<CommonBuildOptions> commonBuildOptions=std::make_shared<::CommonBuildOptions>();
    std::string defaultMountsFilePath;
    std::string iidfile;
    bool squash=false;
    std::vector<std::string> labels;
    std::vector<std::string> layerLabels;
    std::vector<std::string> annotations;
    bool layers=false;
    bool noHostname=false;
    bool noHosts=false;
    bool useCache=false;
    bool removeIntermediateCtrs=false;
    bool forceRmIntermediateCtrs=false;
    std::map<std::string, std::string> imageMap; // Used to map images that we create to handle the AS construct.
    std::map<std::string, std::shared_ptr<Builder>> containerMap; // Used to map from image names to only-created-for-the-rootfs containers.
    std::set<std::string> baseMap; // Holds the names of every base image, as given.
    std::set<std::string> rootfsMap; // Holds the names of every stage whose rootfs is referenced in a COPY or ADD instruction.
    std::string blobDirectory;
    std::vector<std::string> excludes;
    std::vector<std::string> groupAdd;
    std::string ignoreFile;
    std::map<std::string, std::string> args;
    std::map<std::string, std::string> globalArgs;
    std::set<std::string> unusedArgs;
    std::vector<std::string> capabilities;
    std::shared_ptr<ContainerDevices> devices=std::make_shared<::ContainerDevices>();
    std::vector<std::string> deviceSpecs;
    std::string signBy;
    std::string architecture;
    std::shared_ptr<std::chrono::system_clock::time_point> timestamp=std::make_shared<std::chrono::system_clock::time_point>(); // Uses `std::tm` instead of `*time.Time`
    std::string os;
    int maxPullPushRetries=0;
    std::chrono::duration<int> retryPullPushDelay;
    std::shared_ptr<DecryptConfig> ociDecryptConfig=std::make_shared<::DecryptConfig>();
    std::string lastError;
    std::map<std::string, std::string> terminatedStage;
    std::mutex stagesLock;
    std::shared_ptr<Weighted> stagesSemaphore=std::make_shared<::Weighted>();
    bool logRusage=false;
    std::ostream* rusageLogFile=nullptr;
    std::mutex imageInfoMutex;
    std::map<std::string, imageTypeAndHistoryAndDiffIDs> imageInfoCache;
    std::string fromOverride;
    std::map<std::string, std::shared_ptr<AdditionalBuildContext>> additionalBuildContexts;
    std::string manifest;
    std::map<std::string, std::shared_ptr<Secret>> secrets;
    std::map<std::string, std::shared_ptr<Source>> sshsources;
    std::string logPrefix;
    std::vector<std::string> unsetEnvs;
    std::vector<std::string> unsetLabels;
    std::string processLabel; // Shares processLabel of first stage container with containers of other stages in same build
    std::string mountLabel; // Shares mountLabel of first stage container with containers of other stages in same build
    std::string buildOutput; // Specifies instructions for any custom build output
    std::string osVersion;
    std::vector<std::string> osFeatures;
    std::vector<std::string> envs;
    std::shared_ptr<ConfidentialWorkloadOptions> confidentialWorkload=std::make_shared<::ConfidentialWorkloadOptions>();
    std::vector<SBOMScanOptions> sbomScanOptions;
    std::string cdiConfigDir;
    Executor()=default;
    std::tuple<string,std::shared_ptr<Canonical_interface>> Build(std::shared_ptr<Stages> stages);
    void warnOnUnsetBuildArgs(
        const std::vector<Stage>& stages,
        const std::map<std::string, std::shared_ptr<stageDependencyInfo>>& dependencyMap,
        const std::map<std::string, std::string>& args
    );
    std::shared_ptr<ImageReference_interface> resolveNameToImageRef(const std::string& output);
    std::tuple<std::string,std::shared_ptr<Canonical_interface>,bool,std::string> 
    buildStage(
        std::map<int,std::shared_ptr<StageExecutor>> cleanupStages,
        std::shared_ptr<Stages> stages,
        int stageIndex
    );
    std::shared_ptr<StageExecutor> startStage(
        std::shared_ptr<Stage> stage,
        std::shared_ptr<Stages> stages,
        std::string output
    );
    std::pair<bool, std::string> waitForStage(
        const std::string& name, 
        std::shared_ptr<Stages> stages
    );

};

struct stageDependencyInfo{
    std::string Name;
    int Position=0;
    std::vector<std::string> Needs;
    bool NeededByTarget=false;
    stageDependencyInfo()=default;
    stageDependencyInfo(std::string name, int position):Name(name),Position(position){};
    

};
std::shared_ptr<Executor> 
newExecutor(
    std::string logPrefix,
    std::shared_ptr<Store> stores,
    std::shared_ptr<define_BuildOptions> options,
    std::shared_ptr<Node> mainNode,
    std::vector<std::string> containerFiles);
    
void markDependencyStagesForTarget(
    std::map<std::string, std::shared_ptr<stageDependencyInfo>>& dependencyMap,
    const std::string& stage
);


#endif // IMAGEBUILDAH_EXECUTOR_H
