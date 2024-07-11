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
#include "command.h"
/**
 * @brief Command类的列表构造函数
 * <p>对命令的名称，介绍，使用例子进行初始化的构造函数
 * @param name 命令的名称
 * @param Short 命令的简短介绍
 * @param Long 命令详细的介绍
 * @param example 命令的使用样例
 */

Command::Command(string& name,string& Short,string& Long, string& example):
                name(name),Short(Short),Long(Long),example(example) {}
/**
 * @brief 销毁Command::Command对象
 * <p>主要是对自定义的对象指针进行销毁，注意销毁的顺序，避免重复释放资源
 */
Command::~Command(){
    for (Command* cmd : Son_command) {
        delete cmd;
    }
    // 释放动态分配的 Flagset 对象
    // delete flags;
    // delete persistent_flags;
    delete local_flags;
    delete inherited_flags;
    delete parent_persistent_flags;

    // 将指针设为 nullptr
    // parent_Command = nullptr;
    // helpCommand = nullptr;
    // flags = nullptr;
    // persistent_flags = nullptr;
    // local_flags = nullptr;
    // inherited_flags = nullptr;
    // parent_persistent_flags = nullptr;
}
/**
 * @brief CommandLine 是默认的命令行标志集。
 * 
 */
Flagset* CommandLine=NewFlagSet("buildah");

/**
 * @brief 返回命令的标志集
 * 
 * @return Flagset* 标志集的指针
 */
Flagset* Command::Flags(){
    if(flags==nullptr){
        flags=NewFlagSet(name);
        // flags->name=this->name;
    }
    return flags;
}

/**
 * @brief 返回命令的持久化标志集
 * 
 * @return Flagset& 返回命令中持久化标志集的引用
 */
Flagset* NewFlagSet(const string& name){
    Flagset* ret_flag=new Flagset();
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
Flagset* Command::PersistentFlags(){
    if(persistent_flags==nullptr){
        persistent_flags=NewFlagSet(name);
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
    Command* cmd;///<用来分析子命令，例如build
    try
    {
        if(TraverseChildren){
            // Traverse(args,cmd,flags);
            // flags=f;cmd=c;
        }else{
            cmd=Find(args,flags);
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
        throw ;
    }
    
    
}
/**
 * @brief 向Command对象中添加子Command对象
 * <p>函数接受同时添加多个子Command对象
 * @param cmdlist Command对象指针列表
 */
void Command::AddCommand(initializer_list<Command*>cmdlist){
    for (auto x : cmdlist) {
        if (x == this) {
            // throw std::runtime_error("Command can't be a child of itself");
            cerr<<"Command can't be a child of itself"<<endl;
        }
        x->parent_Command=this;
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
void Command::RemoveCommand(initializer_list<Command*>cmdlist){
    // vector<Command> remainingCommands;
    for(auto cmd: cmdlist){
        auto it=remove_if(Son_command.begin(),Son_command.end(),[cmd](Command* command){
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
    Helpfunc()(*this,vector<string>{});
}
/**
 * @brief 一个help函数，用来第一help命令的行为
 * 
 * @param cmd 执行help命令的对象
 * @param a 参数列表
 */
void help_func(Command& cmd, vector<string> a){
    cmd.mergePersistentFlags();
    // The help should be sent to stdout
    //err := tmpl(c.OutOrStdout(), c.HelpTemplate(), c)
}
/**
 * @brief Helpfunc函数返回一个函数指针
 * HelpFunc 返回 SetHelpFunc 为此命令设置的函数或父级，或者返回具有默认帮助行为的函数。
 */
void (* Command::Helpfunc())(Command& cmd, vector<string> str){
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
/**
 * @brief Root函数返回Command对象命令树的根命令
 * 
 * @return Command* 根命令的指针 
 */
Command* Command::Root(){
    if(HasParent()){
        return Parent()->Root();
    }
    return this;
}
/**
 * @brief Parent返回Command对象的父命令
 * 
 * @return Command* 父命令指针
 */
Command* Command::Parent(){
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
        Command* helpcmd=new Command(name,Short,Long,example);
        helpcmd->Run=[](Command& cmd, vector<string> args){
            Command* new_cmd;
            vector<string> new_args;
            new_cmd=cmd.Root()->Find(args,new_args);
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
        string usage("help for");
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
void Command::Traverse(vector<string>args,Command& ret_cmd,vector<string>&ret_args){

}
/**
 * @brief 检查name标签有没有默认值
 * 
 * @param name 标签名
 * @param flags 标签集
 * @return true 
 * @return false 
 */
bool hasNoOptDefVal( string name, Flagset* flags){
    Flag* f=flags->Lookup(name);
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
vector<string> stripFlags(vector<string> args,Command* cmd){
    if(args.size()==0){
        return args;
    }
    cmd->mergePersistentFlags();
    Flagset* flags=cmd->Flags();
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
Command* innerfind(Command* cmd,vector<string>&args,vector<string>& ret_args){
    vector<string>argsWOflags=stripFlags(args,cmd);
    if(argsWOflags.size()==0){
        // ret_cmd=*cmd;
        ret_args=args;
        return cmd;
        // return make_tuple(cmd,args);
    }
    string nextSubCmd = argsWOflags[0];
    Command* next_cmd=cmd->findNext(nextSubCmd);
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
    Flagset* flags= Flags();
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
void legacyArgs(Command*cmd,vector<string>args){
    if(!cmd->HasSubCommands()){
        return;
    }
    if(!cmd->HasParent() && args.size()>0){
        throw myerror("unknown command"+args[0]+" for "+cmd->CommandPath());
    }
    return;
}
/**
 * @brief 根据给定的参数和命令树查找目标命令
 * <p>在最高节点上运行。只能往下搜索。
 * @param ret_args 查找解析的参数
 * @return Command* 查找到的Command对象
 */
Command* Command::Find(vector<string>args,vector<string>&ret_args){
    // Command commandFound;
    // vector<string> new_args;
    Command*commandFound= innerfind(this,args,ret_args);
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
 * @brief 根据next在子命令中查找
 * 
 * @param next Command对象名
 * @return Command* 目标Command对象的指针
 */
Command* Command::findNext(string next){
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
 * @brief 根据解析出的参数执行命令
 * 
 * @param args 参数列表
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
        return;
    }
    if(!Runnable()){
        return;
    }
    // preRun();
    vector<string> argWoFlags = Flags()->Args();
    if(DisableFlagParsing){
        argWoFlags=args;
    }
    

    for(auto p=this;p!=nullptr;p=p->Parent()){
        if(p->PersistentPreRun!=nullptr){
            try
            {
                p->PersistentPreRun(*this,argWoFlags);
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
            PreRun(*this,argWoFlags);
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
            Run(*this,argWoFlags);
        }
        catch(const myerror& e)
        {
            throw;
        }
    }
    if(PostRun!=nullptr){
        try
        {
            PostRun(*this,argWoFlags);
        }
        catch(const myerror& e)
        {
            throw;
        }
    }
    for (auto p=this;p!=nullptr;p=p->Parent()){
        if(p->PersistentPostRun!=nullptr){
            try
            {
                p->PersistentPostRun(*this,argWoFlags);
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
 * @brief ParseFlags 解析持久标志树和本地标志。
 * 
 * @param args 输入参数列表
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
 * @brief ValidateRequiredFlags 验证所有必需的标志是否存在，否则返回错误
 * 
 * @return true 
 * @return false 
 */
bool Command::ValidateRequiredFlags(){
    auto flags=Flags();
    vector<string> missingFlagNames;
    
    function<void(Flag*)> fn=[&missingFlagNames](Flag* f){

    };
    flags->VisitAll(fn);
    if(missingFlagNames.size()>0){
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
 * @brief mergePersistentFlags将PersistentFlags()合并到Flags(),并添加所有父级缺少的持久标志。
 * 
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
    VisitParents([this](Command* parent){
        this->parent_persistent_flags->AddFlagSet(parent->PersistentFlags());
    });
}
/**
 * @brief VisitParents 访问命令的所有父级并在每个父级上调用 fn。
 * 
 * @param fn 上级传递的函数对象
 */
void Command::VisitParents(const function<void(Command*)>& fn){
    if (HasParent()){
        fn(Parent());
        Parent()->VisitParents(fn);
    }
}