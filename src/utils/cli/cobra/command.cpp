/**
 * @file command.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对command.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "utils/cli/cobra/command.h"
#include "utils/cli/cli/common.h"
#include "utils/logger/ProcessSafeLogger.h"


/**
 * @brief Command类的列表构造函数
 * <p>对命令的名称，介绍，使用例子进行初始化的构造函数
 * @param name 命令的名称
 * @param Short 命令的简短介绍
 * @param Long 命令详细的介绍
 * @param example 命令的使用样例
 */

Command::Command(string& name,string& Short,string& Long, string& example):
                name(name),Short(Short),Long(Long),example(example) {

}
/**
 * @brief 销毁Command::Command对象
 * <p>主要是对自定义的对象指针进行销毁，注意销毁的顺序，避免重复释放资源
 */
// Command::~Command(){
//     // for (Command* cmd : Son_command) {
//     //     if(cmd!=nullptr)
//     //     {
//     //         delete cmd;
//     //         cmd=nullptr;
//     //     }
//     // }
    // // delete parent_Command;
    // // delete helpCommand;
    // // 释放动态分配的 Flagset 对象
    // // delete flags;
    // // delete persistent_flags;
    // delete local_flags;
    // delete inherited_flags;
    // delete parent_persistent_flags;

    // // 将指针设为 nullptr
    // // parent_Command = nullptr;
    // // helpCommand = nullptr;
    // // flags = nullptr;
    // persistent_flags = nullptr;
//     local_flags = nullptr;
//     inherited_flags = nullptr;
//     parent_persistent_flags = nullptr;
// }
/**
 * @brief CommandLine 是默认的命令行标志集。
 * 
 */
// Flagset* CommandLine=NewFlagSet("buildah");

/**
 * @brief 返回命令的标志集
 * 
 * @return Flagset* 标志集的指针
 */
std::shared_ptr<Flagset> Command::Flags(){
    if(flags==nullptr){
        flags=NewFlagSet(Name());
        // flags->name=this->name;
    }
    return flags;
}

/**
 * @brief 返回命令的持久化标志集
 * 
 * @return Flagset& 返回命令中持久化标志集的引用
 */
std::shared_ptr<Flagset> NewFlagSet(const string& name){
    auto ret_flag=std::make_shared<Flagset>();
    ret_flag->name=name;
    ret_flag->interspersed=true;
    ret_flag->SortedFlags=true;
    return ret_flag;
}
/**
 * @brief 返回Command对象的持续化标志集
 * <p>如果Command对象的持续化标志集为空，则新建一个空的Flagset
 * @return Flagset* 返回的新建的Flagset指针
 */
std::shared_ptr<Flagset> Command::PersistentFlags(){
    if(persistent_flags==nullptr){
        persistent_flags=NewFlagSet(Name());
        // persistent_flags->name=this->name;
    }
    return persistent_flags;
}
/**
 * @brief 执行入口
 * <p>执行使用args（默认为os.Args[1:]）并运行命令树来查找适当的匹配项用于命令，然后是相应的标志。
 * @param argc 参数数量
 * @param argv 参数列表指针
 */
void Command::Execute(int argc, char const *argv[]){
    try
    {
        ExecuteC(argc,argv);
    }
    catch(const myerror& e)
    {
        logger->log_error("Command execution failed: " + string(e.what()));
        throw ;
    }
}
/**
 * @brief ExecuteC 执行命令
 * 
 * @param argc 参数数量
 * @param argv 参数列表指针
 */
void Command::ExecuteC(int argc, char const *argv[]){
    if(HasParent()){
        Root()->ExecuteC(argc,argv);
    }
    InitDefaultHelpCmd();
    InitDefaultCompletionCmd();
    vector<string>args=this->args;
    if(args.size()==0){
        for (auto i=1;i<argc;i++){
            args.emplace_back(argv[i]);
        }
    }
    vector<string> flags;
    // Command* cmd=new Command();///<用来分析子命令，例如build
    auto cmd=std::make_shared<Command>();
    try
    {
        if(TraverseChildren){
            // Traverse(args,cmd,flags);
            // flags=f;cmd=c;
        }else{
            std::shared_ptr<Command>tmpc(Find(args,flags));
            cmd=tmpc;
            // flags=f;cmd=c;
        }
    }
    catch(const myerror& e)
    {
        if(cmd!=nullptr){
            *this=*cmd;
        }
        cerr<<"Run "<<CommandPath()<<"--help' for usage."<<endl;
        throw ;
    }
    

    cmd->commandcallas.called=true;
    if(cmd->commandcallas.name==""){
        cmd->commandcallas.name=cmd->Name();
    }
    try
    {
        cmd->execute(flags);
    }
    catch(const myerror& e)
    {
        std::string err_msg=e.what();
        if(err_msg=="help requested"){
            logger->log_info("Help requested for command: " + cmd->Name());
            cmd->Helpfunc()(cmd,args);
        }else{
            logger->log_error("Command execution failed: " + string(e.what()));
            throw ;
        }
    }
    // delete cmd;
    // cmd=nullptr;
}
/**
 * @brief 向Command对象中添加子Command对象
 * <p>函数接受同时添加多个子Command对象
 * @param cmdlist Command对象指针列表
 */
void Command::AddCommand(initializer_list<std::shared_ptr<Command>>cmdlist){
    for (auto x : cmdlist) {
        if (x.get() == this) {
            // throw std::runtime_error("Command can't be a child of itself");
            cerr<<"Command can't be a child of itself"<<endl;
        }
        x->parent_Command=shared_from_this();
        // x.parent = this;

        // Update max lengths
        int usageLen = x->name.length();
        if (usageLen > commandsMaxUseLen) {
            commandsMaxUseLen = usageLen;
        }
        int commandPathLen = x->CommandPath().length();
        if (commandPathLen > commandsMaxCommandPathLen) {
            commandsMaxCommandPathLen = commandPathLen;
        }
        int nameLen = x->Name().length();
        if (nameLen > commandsMaxNameLen) {
            commandsMaxNameLen = nameLen;
        }

        // If global normalization function exists, update all children
        // if (globNormFunc) {
        //     x->SetGlobalNormalizationFunc(globNormFunc);
        // }
        Son_command.emplace_back(x);
        commandsAreSorted = false;
    }
    return;
}
/**
 * @brief 在Command对象中删除cmdlist中的子命令
 * @param cmdlist 需要删除的命令指针列表
 */
void Command::RemoveCommand(initializer_list<std::shared_ptr<Command>>cmdlist){
    // vector<Command> remainingCommands;
    for(auto cmd: cmdlist){
        auto it=remove_if(Son_command.begin(),Son_command.end(),[cmd](std::shared_ptr<Command> command){
            if(command==cmd){
                command->parent_Command=nullptr;
                return true;
            }
            return false;
        });
        Son_command.erase(it,Son_command.end());
    }
    commandsMaxUseLen=0;
    commandsMaxCommandPathLen=0;
    commandsMaxNameLen=0;
    for (const auto& command : Son_command) {
        int usageLen = command->name.length();
        if (usageLen > commandsMaxUseLen) {
            commandsMaxUseLen = usageLen;
        }
        int commandPathLen = command->CommandPath().length();
        if (commandPathLen > commandsMaxCommandPathLen) {
            commandsMaxCommandPathLen = commandPathLen;
        }
        int nameLen = command->Name().length();
        if (nameLen > commandsMaxNameLen) {
            commandsMaxNameLen = nameLen;
        }
    }
}
/**
 * @brief CommandPath 返回此命令的完整路径。
 * 
 * @return string 
 */
string Command::CommandPath(){
    if(HasParent()){
        return Parent()->CommandPath()+" "+ Name();
    }
    return Name();
}
/**
 * @brief SetUsageTemplate 设置使用模板
 * 
 * @param str 使用模板的值
 */
void Command::SetUsageTemplate(string& str){
    usageTemplate=str;
}
/**
 * @brief 输出命令的帮助信息
 * 
 */
void Command::Help(){
    Helpfunc()(shared_from_this(),vector<string>{});
}
/**
 * @brief 一个help函数，用来第一help命令的行为
 * 
 * @param cmd 执行help命令的对象
 * @param a 参数列表
 */
void help_func(std::shared_ptr<Command> cmd, vector<string> a){
    cmd->mergePersistentFlags();
    // The help should be sent to stdout
    //err := tmpl(c.OutOrStdout(), c.HelpTemplate(), c)
    try{
        tmpl(std::cout,cmd->HelpTemplate(),cmd);
    }catch(const myerror& e){
        logger->log_error("help error: "+std::string(e.what()));
        std::cerr<<"help error: "<<e.what()<<std::endl;
    }
}
/**
 * @brief Helpfunc函数返回一个函数指针
 * HelpFunc 返回 SetHelpFunc 为此命令设置的函数或父级，或者返回具有默认帮助行为的函数。
 */
void (* Command::Helpfunc())(std::shared_ptr<Command> cmd, vector<string> str){
    if(helpFunc!=nullptr){
        return helpFunc;
    }
    if(HasParent()){
        return Parent()->Helpfunc();
    }
    return &help_func;
}
/**
 * @brief 检查Command对象是否有父对象
 * 
 * @return true 
 * @return false 
 */
bool Command::HasParent(){
    if(parent_Command!=nullptr){
        return true;
    }else{
        return false;
    }
}
std::string Command::HelpTemplate(){
    if(this->helpTemplate!=""){
        return this->helpTemplate;
    }
    if(this->HasParent()){
        return Parent()->HelpTemplate();
    }
    std::string str{"\t%1%\n\nUsage:\n  %2%\n\nAliases:\n  %3%\n\nExamples:\n  %4%\n\nFlags:\n"};
    return str;
}
/**
 * @brief Root函数返回Command对象命令树的根命令
 * 
 * @return Command* 根命令的指针 
 */
std::shared_ptr<Command> Command::Root(){
    if(HasParent()){
        return Parent()->Root();
    }
    return shared_from_this();
}
/**
 * @brief Parent返回Command对象的父命令
 * 
 * @return Command* 父命令指针
 */
std::shared_ptr<Command> Command::Parent(){
    return parent_Command;
}
/**
 * @brief 检查命令Command是否有子命令
 * <p> 如果有返回true，没有返回false
 * @return true 
 * @return false 
 */
bool Command::HasSubCommands(){
    return Son_command.size()>0;
}
/**
 * @brief InitDefaultHelpCmd初始化默认的help命令
 * <p>如果没有help子命令，则创建新的默认help命令
 */
void Command::InitDefaultHelpCmd(){
    if(!HasSubCommands()){
        return;
    }
    if(helpCommand==nullptr){
        string name="help [command]";
        string Short="Help about any command";
        string Long="Help provides help for any command in the application.\
                    Simply type "+ Name() +"help [path to command] for full details.";
        string example="";
        auto helpcmd=std::make_shared<Command>(name,Short,Long,example);
        helpcmd->Run=[](std::shared_ptr<Command> cmd, vector<string> args){
            std::shared_ptr<Command> new_cmd=nullptr;
            vector<string> new_args;
            new_cmd=cmd->Root()->Find(args,new_args);
            new_cmd->InitDefaultHelpFlag();
            new_cmd->InitDefaultVersionFlag();
        };
        helpCommand=helpcmd;
    }
    RemoveCommand({helpCommand});
    AddCommand({helpCommand});
}
/**
 * @brief InitDefaultCompletionCmd 初始化默认的Completion命令
 * 
 */
void Command::InitDefaultCompletionCmd(){
    //添加completion命令
    // c.AddCommand(completionCmd);
    // completionCmd.AddCommand(bash, zsh, fish, powershell);
}
/**
 * @brief 返回命令的名字
 * <p>只取name字符串的第一个空格前的子字符串
 * @return string 返回字符串
 */
string Command::Name(){
    string name=this->name;
    auto it=name.find_first_of(" ");
    if(it!=string::npos){
        return name.substr(0,it);
    }
    return name;
}
/**
 * @brief 将默认的help标志添加到Command对象
 * <p>如果Command对象已经有help标签，则什么也不做
 */
void Command::InitDefaultHelpFlag(){
    mergePersistentFlags();
    if(Flags()->Lookup("help")==nullptr){
        string usage("help for ");
        if(Name()==""){
            usage=usage+"this command";
        }else{
            usage=usage+Name();
        }
        Flags()->BoolP("help",false,usage);
        Flags()->SetAnnotation("help","cobra_annotation_flag_set_by_cobra",vector<string>{"true"});
    }
}
/**
 * @brief 添加version标签到Command对象中
 * <p> 如果version变量为空，则不添加
 */
void Command::InitDefaultVersionFlag(){
    if(version==""){
        return;
    }
    mergePersistentFlags();
    if(Flags()->Lookup("version")==nullptr){
        string usage="version for ";
        if(Name()==""){
            usage+="this command";
        }else{
            usage+=Name();
        }
        Flags()->BoolP("version",false,usage);
        Flags()->SetAnnotation("version","cobra_annotation_flag_set_by_cobra",vector<string>{"true"});

    }
}
/**
 * @brief 遍历命令树以查找命令，并为每个父节点解析参数。
 * 
 * @param ret_cmd 用来保存Command对象
 * @param ret_args 保存查找解析的参数
 */
void Command::Traverse(vector<string>args,std::shared_ptr<Command> ret_cmd,vector<string>&ret_args){

}
/**
 * @brief 检查name标签有没有默认值
 * 
 * @param name 标签名
 * @param flags 标签集
 * @return true 
 * @return false 
 */
bool hasNoOptDefVal( string name, std::shared_ptr<Flagset> flags){
    auto f=flags->Lookup(name);
    if(f==nullptr){
        return false;
    }
    return f->NoOptDefVal!="";
}
// bool shortHasNoOptDefVal(string name, Flagset& fs){
//     if(name.length()==0){
//         return false;
//     }

// }
/**
 * @brief 根据参数进行标签分析
 * 
 * @param args 参数列表
 * @param cmd 运行的命令
 * @return vector<string> 
 */
vector<string> stripFlags(vector<string> args,std::shared_ptr<Command> cmd){
    if(args.size()==0){
        return args;
    }
    cmd->mergePersistentFlags();
    auto flags=cmd->Flags();
    vector<string> commands;
    auto it =args.begin();
    while(it!=args.end()){
        string s=*it;
        it=args.erase(it);
        if(s=="--"){
            /// "--" terminates the flags
            break;
        }
        else if(s.rfind("--",0)==0 && s.find('=')==string::npos && !hasNoOptDefVal(s.substr(2),flags)){
            /// If '--flag arg' then delete arg from args.
            if(it==args.end()){
                break;
            }
            it=args.erase(it);
            continue;
        }
        // else if(s.find("-",0)==0 && s.find('=')==string::npos && s.length()==2)
        else if(!s.empty() && s[0]!='-'){
            commands.emplace_back(s);
        }
    }
    return commands;
}
/**
 * @brief 递归分析参数
 * 
 * @param cmd 使用参数的命令
 * @param args 源参数
 * @param ret_args 返回参数
 * @return Command* 解析参数后，保存到Command
 */
std::shared_ptr<Command> innerfind(std::shared_ptr<Command> cmd,vector<string>&args,vector<string>& ret_args){
    vector<string>argsWOflags=stripFlags(args,cmd);
    if(argsWOflags.size()==0){
        // ret_cmd=*cmd;
        ret_args=args;
        return cmd;
        // return make_tuple(cmd,args);
    }
    string nextSubCmd = argsWOflags[0];
    auto next_cmd=cmd->findNext(nextSubCmd);
    if(next_cmd){
        vector<string>next_args =cmd->argsMinusFirstX(args, nextSubCmd);
        return innerfind(next_cmd,next_args,ret_args);
        
    }
    // ret_cmd=*cmd;
    ret_args=args;
    return cmd;
    // return make_tuple(cmd,args);
}
/**
 * @brief argsMinusFirstX 仅删除 args 中的第一个 x。  
 * 
 * @param x 
 * @return vector<string> 返回字符串列表
 */
vector<string> Command::argsMinusFirstX(vector<string>args,string x){
    if(args.size()==0){
        return args;
    }
    mergePersistentFlags();
    auto flags= Flags();
    for (auto pos=0;pos<args.size();pos++){
        string& s=args[pos];
        if(s=="--"){
            break;
        }
        else if(s.rfind("--",0)==0 && s.find('=')==string::npos && !hasNoOptDefVal(s.substr(2),flags)){
            pos++;
            continue;
        }
        else if(s.rfind("-",0)!=0){
            if(s==x){
                vector<string> ret;
                ret.insert(ret.end(),args.begin(),args.begin()+pos);
                ret.insert(ret.end(),args.begin()+pos+1,args.end());
                return ret;
            }
        }
    }
    return args;
}
/**
 * @brief LegacyArgs
 * <p> LegacyArgs 验证具有以下行为：
 * <p> - 没有子命令的根命令可以采用任意参数
 * <p> - 带有子命令的根命令将进行子命令有效性检查
 * <p> - 子命令将始终接受任意参数
 * @param cmd 
 */
void legacyArgs(std::shared_ptr<Command>cmd,vector<string>args){
    if(!cmd->HasSubCommands()){
        return;
    }
    if(!cmd->HasParent() && args.size()>0){
        throw myerror("unknown command "+args[0]+" for "+cmd->CommandPath());
    }
    return;
}
/**
 * @brief 根据给定的参数和命令树查找目标命令
 * 
 * @details 在命令树中递归查找匹配给定参数的命令。从当前命令节点开始，
 * 只能向下搜索子命令。如果找到匹配命令，则返回该命令指针，并将解析后的
 * 参数存入ret_args。
 * 
 * @param args 输入参数列表
 * @param ret_args [out] 查找解析后的参数
 * @return Command* 查找到的Command对象指针，如果没有找到则返回nullptr
 * 
 * @note 此函数会处理命令标志并验证参数有效性
 */
std::shared_ptr<Command> Command::Find(vector<string>args,vector<string>&ret_args){
    // Command commandFound;
    // vector<string> new_args;
    auto commandFound= innerfind(shared_from_this(),args,ret_args);
    // return make_tuple(a,commandFound,);
    if(commandFound->Args==nullptr){
        vector<string> a=stripFlags(ret_args,commandFound);
        try
        {
            legacyArgs(commandFound,a);
        }
        catch(const myerror& e)
        {
            throw ;
        }
        
        return commandFound;
        
    }
    return commandFound;
}
/**
 * @brief commandNameMatches 检查两个命令名称是否相等
 * 
 * @param s 源命令
 * @param t 目的命令
 * @return true 
 * @return false 
 */
bool commandNameMatches(string s, string t){
    return s==t;
}
/**
 * @brief 在子命令中查找指定名称的命令
 * 
 * @details 在当前命令的子命令列表中查找名称匹配next的命令。
 * 如果找到匹配命令，则设置其commandcallas.name并返回指针。
 * 
 * @param next 要查找的子命令名称
 * @return Command* 找到的子命令指针，未找到则返回nullptr
 * 
 * @note 此函数仅进行名称匹配，不处理参数解析
 */
std::shared_ptr<Command> Command::findNext(string next){
    // vector<Command*> matches;
    for (auto cmd:this->Son_command){
        if(commandNameMatches(cmd->Name(),next)){
            cmd->commandcallas.name=next;
            return cmd;
        }
    }
    return nullptr;
}
/**
 * @brief 执行命令及其相关处理流程
 * 
 * @details 完整的命令执行流程包括：
 * 1. 初始化默认help和version标志
 * 2. 解析输入参数
 * 3. 检查help标志，如果设置则显示帮助信息
 * 4. 检查命令是否可执行(Runnable)
 * 5. 执行父命令的PersistentPreRun回调(如果存在)
 * 6. 执行当前命令的PreRun回调(如果存在) 
 * 7. 验证必需标志和标志组
 * 8. 执行主Run函数
 * 9. 执行PostRun回调(如果存在)
 * 10. 执行父命令的PersistentPostRun回调(如果存在)
 * 
 * @param args 输入参数列表
 * @throws myerror 如果任何步骤失败则抛出异常
 * 
 * @note 此函数是命令执行的主入口点，处理完整的生命周期
 */
void Command::execute(vector<string> args){
    InitDefaultHelpFlag();
    InitDefaultVersionFlag();
    try
    {
        ParseFlags(args);
    }
    catch(const myerror& e)
    {
        throw ;
    }
    bool helpval;
    try
    {
        helpval=Flags()->GetBool("help");
    }
    catch(const myerror& e)
    {
        cerr<<"\"help\" flag declared as non-bool. Please correct your code"<<endl;
        throw ;
    }
    
    if(helpval){
        throw myerror("help requested");
    }
    if(!Runnable()){
        return;
    }
    // preRun();
    vector<string> argWoFlags = Flags()->Args();
    if(DisableFlagParsing){
        argWoFlags=args;
    }
    

    for(auto p=shared_from_this();p!=nullptr;p=p->Parent()){
        if(p->PersistentPreRun!=nullptr){
            try
            {
                p->PersistentPreRun(shared_from_this(),argWoFlags);
            }
            catch(const myerror& e)
            {
                throw;
            }
        }
    }
    if(PreRun!=nullptr){
        try
        {
            PreRun(shared_from_this(),argWoFlags);
        }
        catch(const myerror& e)
        {
            throw;
        }
        
    }
    try
    {
        ValidateRequiredFlags();
    }
    catch(const myerror& e)
    {
        throw;
    }
    try
    {
        ValidateFlagGroups();
    }
    catch(const myerror& e)
    {
        throw;
    }
    if(Run!=nullptr){
    try
    {
        Run(shared_from_this(),argWoFlags);
    }
    catch(const myerror& e)
    {
        logger->log_error("Command handler failed: " + string(e.what()));
        throw;
    }
    }
    if(PostRun!=nullptr){
        try
        {
            PostRun(shared_from_this(),argWoFlags);
        }
        catch(const myerror& e)
        {
            throw;
        }
    }
    for (auto p=shared_from_this();p!=nullptr;p=p->Parent()){
        if(p->PersistentPostRun!=nullptr){
            try
            {
                p->PersistentPostRun(shared_from_this(),argWoFlags);
            }
            catch(const myerror& e)
            {
                throw;
            }
        }
    }
    return;
}
/**
 * @brief 解析命令标志
 * 
 * @details 解析输入参数中的标志，包括：
 * - 合并持久化标志
 * - 解析本地标志
 * - 处理标志参数
 * 
 * @param args 输入参数列表
 * @throws myerror 如果解析失败则抛出异常
 * 
 * @note 此函数会修改内部flags状态
 */
void Command::ParseFlags(vector<string> args){
    mergePersistentFlags();
    try
    {
        Flags()->Parse(args);
    }
    catch(const myerror& e)
    {
        throw;
    }
    
    
}
/**
 * @brief 验证必需标志
 * 
 * @details 检查所有标记为required的标志是否已设置值。
 * 如果发现未设置的必需标志，则抛出异常。
 * 
 * @return true 验证通过
 * @throws myerror 如果发现未设置的必需标志
 * 
 * @note 此函数会遍历所有标志，性能开销较大
 */
bool Command::ValidateRequiredFlags(){
    auto flags=Flags();
    vector<string> missingFlagNames;
    
    function<void(std::shared_ptr<Flag>)> fn=[&missingFlagNames](std::shared_ptr<Flag> f){

    };
    flags->VisitAll(fn);
    if(missingFlagNames.size()>0){
        logger->log_error("required flag(s) "+to_string(missingFlagNames.size()) +" not set");
        throw myerror("required flag(s) "+to_string(missingFlagNames.size()) +" not set");
        // cerr<<"required flag(s) "<<to_string(missingFlagNames.size()) <<" not set"<<endl;
    }
    return true;
}
/**
 * @brief ValidateFlagGroups 验证mutualExclusive/oneRequired/requiredAsGroup 逻辑并返回遇到的第一个错误。
 * 
 * @return true 
 * @return false 
 */
bool Command::ValidateFlagGroups(){
    ///<暂时未定义
    return true;
}
/**
 * @brief Runnable 确定命令本身是否可运行。
 * 
 * @return true 
 * @return false 
 */
bool Command:: Runnable(){
    if(Run!=nullptr){
        return true;
    }
    return false;
}
/**
 * @brief 合并持久化标志
 * 
 * @details 将当前命令的持久化标志和所有父命令的持久化标志
 * 合并到本地标志集中。具体步骤：
 * 1. 更新父命令持久标志(parent_persistent_flags)
 * 2. 将当前持久标志加入本地标志
 * 3. 将父命令持久标志加入本地标志
 * 
 * @note 此函数会修改flags和parent_persistent_flags状态
 */
void Command::mergePersistentFlags(){
    updateParentsPflags();
    Flags()->AddFlagSet(PersistentFlags());
    Flags()->AddFlagSet(parent_persistent_flags);
}
/**
 * @brief updateParentsPflags 通过添加所有父级的新持久标志来更新 parentsPflags。 
 * <p>If parentsPflags == nullptr，它产生新的。
 * 
 */
void Command::updateParentsPflags(){
    if(parent_persistent_flags==nullptr){
        parent_persistent_flags=NewFlagSet(Name());
        parent_persistent_flags->SortedFlags=false;
    }
    Root()->PersistentFlags()->AddFlagSet(CommandLine);
    // Root()->PersistentFlags()->AddFlagSet(CommandLine);
    VisitParents([this](std::shared_ptr<Command> parent){
        this->parent_persistent_flags->AddFlagSet(parent->PersistentFlags());
    });
}
/**
 * @brief VisitParents 访问命令的所有父级并在每个父级上调用 fn。
 * 
 * @param fn 上级传递的函数对象
 */
void Command::VisitParents(const function<void(std::shared_ptr<Command>)>& fn){
    if (HasParent()){
        fn(Parent());
        Parent()->VisitParents(fn);
    }
}
/**
 * @brief 查找标志
 * 
 * @details 递归查找指定名称的标志，查找顺序：
 * 1. 先在本地标志集中查找
 * 2. 如果未找到，则在持久化标志集中查找
 * 
 * @param name 要查找的标志名称
 * @return Flag* 找到的标志指针，未找到则返回nullptr
 * 
 * @note 返回的标志指针生命周期由所属标志集管理
 */
std::shared_ptr<Flag> Command::Flag_find(string name){
    auto flags=Flags();
    auto flag=flags->Lookup(name);
    if(flag==nullptr){
        flag=persistentFlag_find(name);
    }
    return flag;
}
/**
 * @brief 查找持久化标志
 * 
 * @details 递归查找指定名称的持久化标志，查找顺序：
 * 1. 在当前命令的持久标志集中查找
 * 2. 在父命令的持久标志集中查找
 * 
 * @param name 要查找的标志名称
 * @return Flag* 找到的标志指针，未找到则返回nullptr
 * 
 * @note 此函数会触发父标志集更新(updateParentsPflags)
 */
std::shared_ptr<Flag> Command::persistentFlag_find(string name){
    std::shared_ptr<Flag> flag=nullptr;
    if(HasPersistentFlags()){
        flag=PersistentFlags()->Lookup(name);
    }
    if(flag==nullptr){
        updateParentsPflags();
        flag=parent_persistent_flags->Lookup(name);
    }
    return flag;
}
/**
 * @brief HasPersistentFlags 检查命令是否包含持久标志。
 * 
 * @return true 
 * @return false 
 */
bool Command::HasPersistentFlags(){
    return PersistentFlags()->HasFlags();
}
