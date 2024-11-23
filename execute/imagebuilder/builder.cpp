#include "imagebuilder/builder.h"
#include "go/string.h"
#include "cobra/error.h"
#include "cli/common.h"
#include "imagebuilder/internals.h"
#include "imagebuilder/dispatchers.h"

void logExecutor::UnrecognizedInstruction(std::shared_ptr<Step> step){
    return;
}
void logExecutor::Preserve(std::string path){
    return;
}
void logExecutor::EnsureContainerPath(std::string path){
    return;
}
void logExecutor::EnsureContainerPathAs(std::string path,std::string user,const mode_t* mode) {
    return;
}
void logExecutor::COPY(std::vector<std::string> excludes,std::vector<Copy> copies) {
    return;
}
void logExecutor::RUN(std::shared_ptr<Run> run) {
    return;
}


void noopExecutor::UnrecognizedInstruction(std::shared_ptr<Step> step){
    return;
}
void noopExecutor::Preserve(std::string path){
    return;
}
void noopExecutor::EnsureContainerPath(std::string path){
    return;
}
void noopExecutor::EnsureContainerPathAs(std::string path,std::string user,const mode_t* mode) {
    return;
}
void noopExecutor::COPY(std::vector<std::string> excludes,std::vector<Copy> copies) {
    return;
}
void noopExecutor::RUN(std::shared_ptr<Run> run) {
    return;
}
// ParseIgnore returns a tuple containing a list of excludes and an error message
// path should be a file with the .dockerignore format
// extracted from fsouza/go-dockerclient and modified to drop comments and
// empty lines.
std::vector<std::string> ParseIgnore(const std::string& path) {
    std::vector<std::string> excludes;
    std::string error_message;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        error_message = "Could not open file: " + path;
        // return excludes;

        throw myerror(error_message);
    }

    std::string line;
    while (std::getline(file, line)) {
        line = TrimSpace(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        // Remove leading slashes from the path
        if (line[0] == '/') {
            line = line.substr(1);
        }
        if (!line.empty()) {
            excludes.push_back(line);
        }
    }
    
    if (file.bad()) {
        error_message = "Error reading file: " + path;
        // return excludes;
        throw myerror(error_message);
    }

    return excludes;
}

std::shared_ptr<Image_Builder>NewBuilder(std::map<std::string,std::string>args){
    return newBuilderWithGlobalAllowedArgs(args,std::vector<std::string>());
}

std::shared_ptr<Image_Builder>newBuilderWithGlobalAllowedArgs(std::map<std::string,std::string>args,std::vector<std::string>globalallowedargs){
    auto allowed=std::map<string,bool>();
    for(auto& arg:builtinAllowedBuildArgs){
        allowed[arg.first]=arg.second;
    }
    auto userArgs=std::map<string,string>();
    auto initialArgs=std::map<string,string>();

    for(auto& arg:args){
        userArgs[arg.first]=arg.second;
        initialArgs[arg.first]=arg.second;
    }
    auto builder=std::make_shared<Image_Builder>();
    builder->Args=initialArgs;
    builder->UserArgs=userArgs;
    builder->HeadingArgs=std::map<std::string,std::string>();
    builder->AllowedArgs=allowed;
    builder->GlobalAllowedArgs=globalallowedargs;
    return builder;
}

std::shared_ptr<Stages>NewStages(std::shared_ptr<Node>node,std::shared_ptr<Image_Builder>b){
    auto stages=std::make_shared<Stages>();
    std::vector<std::string> allDeclaredArgs;
    auto nodes=SplitBy(node,"arg");
    // auto children=node->Children;
    for(auto root:nodes){
        auto argNode=root->Children[0];
        if(argNode->Value=="arg"){
            auto s=SplitN(argNode->Original," ",2);
            if(s.size()==2 && toLower(s[0])=="arg"){
                allDeclaredArgs.push_back(s[1]);
            }
        }
    }
    try{
        b->extractHeadingArgsFromNode(node);
    }catch(const myerror& e){
        throw;
    }
    auto f=SplitBy(node,"from");
    for(auto i=0;i<f.size();i++){
        auto name=extractNameFromNode(f[i]->Children[0]);
        if(get<0>(name).size()==0){
            get<0>(name)=std::to_string(i);
        }
        auto stage=std::make_shared<Stage>();
        stage->Name=get<0>(name);
        stage->Position=i;
        stage->image_builder=b->builderForStage(allDeclaredArgs);
        stage->Node=f[i];
        stages->Stages.push_back(*stage);
    }
    return stages;
}
bool Image_Builder::RequiresStart(std::shared_ptr<Node> node){
    for(auto child:node->Children){
        if(child->Value=="run"){
            return true;
        }
    }
    return false;
}

std::vector<std::string> Image_Builder::Arguments(){
    std::vector<std::string> envs;
    for(auto it:Args){
        if(AllowedArgs.find(it.first)!=AllowedArgs.end()){
            envs.push_back(it.first+"="+it.second);
        }
    }
    return envs;
}
std::string Image_Builder::From(std::shared_ptr<Node> node){
    try{
        extractHeadingArgsFromNode(node);
    }catch(const myerror& e){
        throw;
    }
    auto children=SplitChildren(node,"from");
    if(children.size()==0){
        throw myerror("no FROM statement found");
    }else if(children.size()>1){
        throw myerror("multiple FROM statements are not supported");
    }else{
        auto step=Step_new();
        auto NoopExecutor=std::make_shared<noopExecutor>();
        try{
            step->Resolve(children[0]);
            Run(step,NoopExecutor,false);
        }catch(const myerror& e){
            throw;
        }
        return RunConfig->Image;
    }
}

void Image_Builder::extractHeadingArgsFromNode(std::shared_ptr<Node> node){
    std::vector<std::shared_ptr<Node>> args;      // 存储与 Arg 相关的节点
    std::vector<std::shared_ptr<Node>> children;  // 存储其余的子节点
    bool extract = true;          // 指示是否继续提取 Arg 节点

    // 遍历节点的子节点
    // std::vector<std::shared_ptr<Node>> childs; 
    // try {
    //     childs = node->Children;
    // } catch (const std::exception& e) {
    //     std::cerr << "Memory allocation failed: " << e.what() << std::endl;
    // }
    for (auto i=0;i<node->Children.size();i++) {
        if (!node->Children[i]) {
            std::cerr << "Warning: child is nullptr" << std::endl;
            continue; // 如果 child 为空则跳过
        }
        if (extract && node->Children[i]->Value == "arg") {
            args.push_back(node->Children[i]);  // 如果是 Arg，添加到 args 中
        } else {
            if (node->Children[i]->Value == "from") {
                extract = false;  // 遇到 From，停止提取 Arg
            }
            children.push_back(node->Children[i]);  // 添加到 children 中
        }
    }
    node->Children=children;
    auto tempBuilder=NewBuilder(UserArgs);

    for (auto c : args) {
        // auto step=tempBuilder->Step();
        // try{
        //     step->Resolve(c);
        //     tempBuilder->Run(step,NoopExecutor,false);
        // }catch(myerror& e){
        //     throw;
        // }
    }
    for(auto v:tempBuilder->Args){

    }

}

std::vector<std::shared_ptr<Node>> SplitBy(std::shared_ptr<Node>node,std::string value){
    std::vector<std::shared_ptr<Node>> split;  // 存储分割后的节点列表
    std::shared_ptr<Node> current = nullptr;   // 当前正在构建的节点

    // 遍历节点的子节点
    for (auto child : node->Children) {
        // 如果 current 为空或遇到与 value 匹配的节点，创建新的 current
        if (current == nullptr || child->Value == value) {
            // auto  copied = *node;  // 复制当前 node
            // current=std::shared_ptr<Node>(&copied);
            current = std::make_shared<Node>(*node);  // 使用 make_shared 复制 node
            current->Children.clear();        // 清空 children 列表
            current->Next = nullptr;          // 清空 next 指针
            // std::shared_ptr<Node> next(&copied);
            // current = next;
            split.push_back(current);        // 添加到分割列表中
        }
        // 将当前子节点添加到 current 的 children 列表中
        current->Children.push_back(child);
    }
    // auto children=node->Children;
    return split;  // 返回分割后的节点列表
}

std::tuple<std::string,bool> extractNameFromNode(std::shared_ptr<Node>node){
    if (node->Value != "from") {
        return std::make_tuple("", false);  // 如果 node 的值不是 "FROM"，返回空字符串和 false
    }

    auto  n = node->Next;
    if (n == nullptr || n->Next == nullptr) {
        return std::make_tuple("", false);  // 如果 next 节点为空或者 next 的 next 为空，返回空字符串和 false
    }

    n = n->Next;
    auto str1 = n->Value;
    std::string str2="as";
    // 检查 n 的值是否为 "as"，并且 n->next 存在且值非空
    bool f=std::equal(str1.begin(), str1.end(), str2.begin(),
                    [](char a, char b) { return tolower(a) == tolower(b); });

    if (!f || n->Next == nullptr || n->Next->Value.empty()) {
        return std::make_tuple("", false);  // 如果不满足条件，返回空字符串和 false
    }

    return std::make_tuple(n->Next->Value, true);  // 返回提取到的名称和 true
}

std::shared_ptr<Image_Builder> Image_Builder::builderForStage(std::vector<std::string> globalArgsList){
    auto stageBuilder=newBuilderWithGlobalAllowedArgs(UserArgs,globalArgsList);
    for (auto a:HeadingArgs){
        stageBuilder->HeadingArgs[a.first]=a.second;
    }
    return stageBuilder;
}

std::vector<std::shared_ptr<Node>> SplitChildren(std::shared_ptr<Node>node,std::string value){
    auto split= std::vector<std::shared_ptr<Node>>();  // 存储分割后的节点列表
    auto children =std::vector<std::shared_ptr<Node>>();
    for(auto child:node->Children){
        if(child->Value==value){
            split.push_back(child);
        }else{
            children.push_back(child);
        }
    }
    node->Children=children;
    return split;
}


std::shared_ptr<Step> Image_Builder::Step_new(){
    auto step=std::make_shared<Step>();
    step->Env=mergeEnv(Arguments(),mergeEnv(Env,RunConfig->Env));
    return step;
}
// using StepFunc=std::function<void(std::shared_ptr<Image_Builder>,std::vector<std::string>,std::map<std::string,bool>,std::vector<std::string>,std::string,std::vector<Heredoc>)>;
// map<std::string,StepFunc> evaluateTable={
//     {dockerfilecommand::Env,env},
//     {dockerfilecommand::From,from},
//     {dockerfilecommand::Label,label},
//     {dockerfilecommand::Copy,dispatchCopy},
//     {dockerfilecommand::Expose,expose},
//     {dockerfilecommand::Entrypoint,entrypoint},
//     {dockerfilecommand::Volume,Volume}
// };
void Image_Builder::Run(std::shared_ptr<Step> step,std::shared_ptr<Executor_Interface> exec,bool noRunsRemaining ){
    auto fn=evaluateTable.find(step->Command);
    if(fn==evaluateTable.end()){
        return exec->UnrecognizedInstruction(step);
    }
    try{
        auto func=fn->second;
        func(std::make_shared<Image_Builder>(*this),step->Args,step->Attrs,step->Flags,step->Original,step->Heredocs);
    }catch(const myerror& e){
        throw;
    }
    auto copies = this->PendingCopies;
	this->PendingCopies.clear();
	auto runs = this->PendingRuns;
	this->PendingRuns.clear();

    for (auto& path : this->PendingVolumes->Volumes){

    }
    try{
        exec->COPY(this->Excludes,copies);
    }catch(const myerror& e){
        throw;
    }
    if(this->RunConfig->WorkingDir.size()>0){
        try{
            exec->EnsureContainerPathAs(this->RunConfig->WorkingDir,this->RunConfig->User,nullptr);
        }catch(const myerror& e){
            throw;
        }
    }
    for (auto& run:runs){

    }
    return;
}
std::shared_ptr<container_Config> Image_Builder::Config(){
    auto config=this->RunConfig;
    if(config->OnBuild.empty()){
        config->OnBuild=std::vector<std::string>();
    }
    if(config->Entrypoint.empty()){
        config->Entrypoint=std::vector<std::string>();
    }
    config->Image="";
    return config;
}
void Image_Builder::FromImage(std::shared_ptr<Docker::Image>image,std::shared_ptr<Node>node){

}
// Add 方法：添加路径并返回是否成功
bool VolumeSet::Add( const std::string& path) {
    if (path == "/") {
        bool set = Volumes.size() != 1 || (Volumes.size() == 1 && Volumes[0] != "");
        Volumes = {""};
        return set;
    }

    std::string trimmedPath = TrimSuffix(path,"/");
    std::vector<std::string> adjusted;

    for (const auto& p : Volumes) {
        if (p == trimmedPath || hasPrefix(trimmedPath ,p + "/") == 0) {
            return false;
        }
        if (hasPrefix(p,trimmedPath + "/") == 0) {
            continue;
        }
        adjusted.push_back(p);
    }

    adjusted.push_back(trimmedPath);
    Volumes = adjusted;
    return true;
}

bool VolumeSet::Has(std::string path){
    if(path=="/"){
        return Volumes.size()==1 && Volumes[0]=="";
    }
    path=TrimSuffix(path,"/");
    for(const auto& p:Volumes){
        if(p==path ){
            return true;
        }
    }
    return false;
}

bool VolumeSet::Covers(std::string path){
    if(path=="/"){
        return Volumes.size()==1 && Volumes[0]=="";
    }
    path=TrimSuffix(path,"/");
    for(const auto& p:Volumes){
        if(p==path||hasPrefix(path,p+"/")==0){
            return true;
        }
    }
    return false;
}







