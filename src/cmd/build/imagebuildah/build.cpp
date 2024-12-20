#include "cmd/build/imagebuildah/build.h"
#include <mutex>
#include <future>
#include "utils/cli/cli/common.h"
#include "utils/parse/parse.h"
#include "utils/logger/logrus/logger.h"
#include <fstream>
#include "utils/common/go/string.h"
#include <sstream>
#include <tuple>
#include "utils/common/semaphore.h"
#include "image/image_types/v1/descriptor.h"
#include "image/types/reference/reference.h"
#include "utils/common/group.h"
#include "cmd/build/imagebuilder/evaluator.h"
#include "cmd/build/imagebuildah/executor.h"
#include "config/config.h"
#include "config/new.h"
std::tuple<std::string,std::shared_ptr<Canonical_interface>> buildDockerfilesOnce(shared_ptr<Store> stores,string logPrefix,shared_ptr<define_BuildOptions> options,vector<string>& containerFiles,vector<vector<byte>>& dockerfilecontents);
string BuildDockerfiles(shared_ptr<Store> stores, shared_ptr<define_BuildOptions> options,vector<string> paths,shared_ptr<Canonical_interface> ret_ref){
    // auto ctx = getContext();

    if (options->CommonBuildOpts == nullptr)
    {
        //将options.CommonBuildOpts初始化重新为CommonBuildOptions 
    }
    try
    {
        Volumes(options->CommonBuildOpts->Volumes);
    }
    catch(const myerror& e)
    {
        throw myerror("validating volumes: "+string(e.what()));
    }
    
    if (paths.empty())
    {
        throw myerror("building: no dockerfiles specified");
    }
    if (options->PlatformsList.size() > 1 && options->IIDFile != "")
    {
        throw myerror ("building multiple images, but iidfile %q can only be used to store one image ID "+ options->IIDFile);
    }
    
    // auto logger = Logger::New();
    // if (options->Err != nullptr)
    // {
    //     logger->SetOutput(options->Err);//基础类结构-改define_BuildOptions
    // }
    // else
    // {
    //     //logger->SetOutput(os.Stderr);
    //     //os库，未执行
    // }
    // auto level = logger->GetLevel();
    // logger->SetLevel(level);
    std::vector<std::shared_ptr<std::fstream>> dockerfiles;
    auto deleter = [&]() {
        for (auto& f : dockerfiles) {
            f->close();
        }
    };
    auto tags=options->AdditionalTags;
    tags.emplace_back(options->Output);
    for (const auto& tag : tags) {
        if (tag.empty()) {
            continue;
        }
        try {
            VerifyTagName(tag);
        } catch (const myerror& e) {
            throw myerror("tag " + tag + " error: " + e.what());
        }
    }
    for (const auto& dfile : paths)
    {
        std::shared_ptr<std::fstream> data;
        if(hasPrefix(dfile,"http://")||hasPrefix(dfile,"https://")){
            
        }else{
            struct stat buf;
            if(stat(dfile.c_str(),&buf)<0){ //判断文件是否存在
                throw myerror("no such file or directory: "+dfile);
            }
            if(buf.st_mode & S_IFDIR){
                throw myerror("file is a directory: "+dfile);
            }
            auto content =fstream(dfile,ios::in|ios::binary);
            if(!content.is_open()){
                throw myerror("failed to open file: "+dfile);
            }
            if(buf.st_mode & S_IFREG && buf.st_size == 0){
                content.close();
                throw myerror("no contents in "+dfile);
            }
            data=std::make_shared<std::fstream>(dfile,ios::in|ios::binary);
            // data=std::make_shared<ifstream>(content);
        }
        if(hasPrefix(dfile,".in")){

        }
        dockerfiles.emplace_back(data);
    }
    auto files=vector<vector<byte>>();
    for (auto& dockerfile : dockerfiles) {
        //默认构造的 std::istreambuf_iterator，表示流的结束（EOF）,后面的it表示结束
        std::vector<byte> buffer((std::istreambuf_iterator<char>(*dockerfile)), std::istreambuf_iterator<char>());
        if(buffer.size()==0){
            std::cout<<"no contents in dockerfile"<<std::endl;
        }
        files.push_back(buffer);
    }
    if(options->JobSemaphore!=nullptr){
        if(options->Jobs!=nullptr){
            if(*(options->Jobs)<0){
                throw myerror("building: invalid value for jobs.  It must be a positive integer");
            }
            if(*(options->Jobs)>0){
                options->JobSemaphore=NewWeighted(static_cast<int64_t>(*options->Jobs));
            }
        }else{
            options->JobSemaphore=NewWeighted(1);
        }
    }
    auto manifestList=options->Manifest;
    options->Manifest="";
    class instance{
        public:
        Platform platform;
        std::string ID;
        std::shared_ptr<Canonical_interface> Ref=nullptr;
    };
    std::vector<instance> instances;
    std::mutex instancesLock;
    Group builds;
    if(options->systemContext==nullptr){

    }
    if(options->PlatformsList.size() == 0){

    }
    if(options->AllPlatforms){

    }
    auto systemContext=options->systemContext;
    for(auto platform : options->PlatformsList){
        auto platformContext=*systemContext;

        auto platformSpec=NormalizePlatform(Platform{platform.OS,platform.Arch,platform.Variant});
        if(platform.OS!=""){

        }
        if(platform.Arch!=""){

        }
        auto platformOptions=options;
        platformOptions->systemContext=make_shared<SystemContext>(platformContext);//&platformContext;
        platformOptions->OS=platformContext.OSChoice;
        platformOptions->Architecture=platformContext.ArchitectureChoice;
        std::string logPrefix="";
        if(options->PlatformsList.size()>1){

        }

        auto argsCopy=std::map<string,string>();
        for(auto arg : options->Args){
            argsCopy[arg.first]=arg.second;
        }
        platformOptions->Args=argsCopy;
        if(platformOptions->LogFile!="" && platformOptions->LogSplitByPlatform){

        }
        std::string thisID;
        std::shared_ptr<Canonical_interface> thisRef;std::cout<<"buildDockerfilesOnce"<<std::endl;
        std::tie(thisID,thisRef)=buildDockerfilesOnce(stores,logPrefix,platformOptions,paths,files);
        instancesLock.lock();
        auto ins=instance();
        ins.ID=thisID;
        ins.Ref=thisRef;
        ins.platform=platformSpec;
        instances.emplace_back(ins);
        instancesLock.unlock();
        // builds.Go([&](){
        //     if(platformOptions->LogFile!="" && platformOptions->LogSplitByPlatform){
        //     }
            // std::string thisID;
            // std::shared_ptr<Canonical_interface> thisRef;
            // std::tie(thisID,thisRef)=buildDockerfilesOnce(stores,logPrefix,platformOptions,paths,files);
            // instancesLock.lock();
            // auto ins=instance();
            // ins.ID=thisID;
        //     ins.Ref=thisRef;
        //     ins.platform=platformSpec;
        //     instances.emplace_back(ins);
        //     instancesLock.unlock();
        // });
    }
    // auto merr=builds.Wait();
    // if(merr!=nullptr){
    //     throw myerror("failed to build: "+string(merr->what()));
    // }
    string id=instances[0].ID;
    ret_ref=instances[0].Ref;
    deleter();
    return id;

}

std::tuple<std::string,std::shared_ptr<Canonical_interface>> buildDockerfilesOnce(shared_ptr<Store> stores,string logPrefix,shared_ptr<define_BuildOptions> options,vector<string>& containerFiles,vector<vector<byte>>& dockerfilecontents){
    auto mainNode=std::make_shared<Node>();
    try
    {
        mainNode=ParseDockerfile(dockerfilecontents[0]);
    }
    catch(const myerror& e)
    {
        throw myerror("parsing main Dockerfile: "+containerFiles[0]+": "+string(e.what()));
    }
    // auto children=mainNode->Children;
    if(options->systemContext->OSChoice!=""&& options->systemContext->ArchitectureChoice!=""){
        
    }
    for(auto it=dockerfilecontents.begin()+1;it!=dockerfilecontents.end();++it){
        
    }
    std::shared_ptr<Executor> exec;
    try{
        exec=newExecutor(logPrefix,stores,options,mainNode,containerFiles);
    }catch(const myerror& e){
        throw myerror("creating build executor: "+std::string(e.what()));
    }
    auto b=NewBuilder(options->Args);
    std::shared_ptr<Config> defaultContainerConfig;
    try{
        defaultContainerConfig=Config_defaut();
    }catch(const myerror& e){
        throw myerror("failed to get container config: "+std::string(e.what()));
    }
    auto defaultenv=defaultContainerConfig->GetDefaultEnv();
    b->Env.insert(b->Env.end(),defaultenv.begin(),defaultenv.end());
    
    std::shared_ptr<Stages> stages;
    try{
        stages=NewStages(mainNode,b);
    }catch(const myerror& e){
        throw myerror("reading multiple stages: "+std::string(e.what()));
    }
    if(options->Target!=""){

    }std::cout<<"exec->Build"<<std::endl;
    return exec->Build(stages);
    // return std::make_tuple("",canonical());
}
