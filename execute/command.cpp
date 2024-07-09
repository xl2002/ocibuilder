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
 * 对命令的名称，介绍，使用例子进行初始化的构造函数
 * @param name 命令的名称
 * @param Short 命令的简短介绍
 * @param Long 命令详细的介绍
 * @param example 命令的使用样例
 */

Command::Command(string& name,string& Short,string& Long, string& example):
                name(name),Short(Short),Long(Long),example(example) {}
Command::~Command(){
    for (Command* cmd : Son_command) {
        delete cmd;
    }
    // 释放动态分配的 Flagset 对象
    delete flags;
    delete persistent_flags;
    delete local_flags;
    delete inherited_flags;
    delete parent_persistent_flags;

    // 将指针设为 nullptr
    parent_Command = nullptr;
    helpCommand = nullptr;
    flags = nullptr;
    persistent_flags = nullptr;
    local_flags = nullptr;
    inherited_flags = nullptr;
    parent_persistent_flags = nullptr;
}

Flagset CommandLine{};

/**
 * @brief 返回命令的标志集
 * 
 * @return Flagset& 返回命令中标志集的引用
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

Flagset* Command::PersistentFlags(){
    if(persistent_flags==nullptr){
        persistent_flags=NewFlagSet(name);
        // persistent_flags->name=this->name;
    }
    return persistent_flags;
}

void Command::Execute(int argc, char const *argv[]){
    ExecuteC(argc,argv);
}

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
    Command cmd;///<用来分析子命令，例如build
    if(TraverseChildren){
        Traverse(args,cmd,flags);
        // flags=f;cmd=c;
    }else{
        Find(args,cmd,flags);
        // flags=f;cmd=c;
    }
    cmd.commandcallas.called=true;
    if(cmd.commandcallas.name==""){
        cmd.commandcallas.name=cmd.name;
    }
    cmd.execute(flags);
}
/**
 * @brief 向命令中添加子命令
 * 
 * @return Command& 返回命令的本身引用
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
string Command::CommandPath(){
    if(HasParent()){
        return Parent()->CommandPath()+" "+ Name();
    }
    return Name();
}
void Command::SetUsageTemplate(string& str){
    usageTemplate=str;
}

void Command::Help(){
    Helpfunc()(*this,vector<string>{});
}
void help_func(Command& cmd, vector<string> a){
    cmd.mergePersistentFlags();
    // The help should be sent to stdout
    //err := tmpl(c.OutOrStdout(), c.HelpTemplate(), c)
}
void (* Command::Helpfunc())(Command& cmd, vector<string> str){
    if(helpFunc!=nullptr){
        return helpFunc;
    }
    if(HasParent()){
        return Parent()->Helpfunc();
    }
    return &help_func;
}

bool Command::HasParent(){
    if(parent_Command){
        return true;
    }else{
        return false;
    }
}
Command* Command::Root(){
    if(HasParent()){
        return Parent()->Root();
    }
    return this;
}
Command* Command::Parent(){
    return parent_Command;
}
bool Command::HasSubCommands(){
    return Son_command.size()>0;
}
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
        Command helpcmd(name,Short,Long,example);
        helpcmd.Run=[](Command& cmd, vector<string>& args){
            Command new_cmd;
            vector<string> new_args;
            cmd.Root()->Find(args,new_cmd,new_args);
            new_cmd.InitDefaultHelpFlag();
            new_cmd.InitDefaultVersionFlag();
        };
        helpCommand=&helpcmd;
    }
    RemoveCommand({helpCommand});
    AddCommand({helpCommand});
}

void Command::InitDefaultCompletionCmd(){
    //添加completion命令
    // c.AddCommand(completionCmd);
    // completionCmd.AddCommand(bash, zsh, fish, powershell);
}
string Command::Name(){
    string name=this->name;
    auto it=name.find_first_of(" ");
    if(it!=string::npos){
        return name.substr(0,it);
    }
    return name;
}
void Command::InitDefaultHelpFlag(){
    mergePersistentFlags();
    if(Flags()->Lookup("help")){
        string usage("help for");
        if(Name()==""){
            usage=usage+"this command";
        }else{
            usage=usage+Name();
        }
        Flags()->BoolP("help",false,usage);
        flags->SetAnnotation("help","cobra_annotation_flag_set_by_cobra",vector<string>{"true"});
    }
}
void Command::InitDefaultVersionFlag(){
    if(version==""){
        return;
    }
    mergePersistentFlags();
    if(Flags()->Lookup("version")){
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
void Command::Traverse(vector<string>args,Command& ret_cmd,vector<string>&ret_args){

}
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
vector<string> stripFlags(vector<string> args,Command& cmd){
    if(args.size()==0){
        return args;
    }
    cmd.mergePersistentFlags();
    Flagset* flags=cmd.Flags();
    vector<string> commands;
    auto it =args.begin();
    while(it!=args.end()){
        string s=*it;
        args.erase(it);
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
void innerfind(Command& cmd,vector<string>&args,Command& ret_cmd,vector<string>& ret_args){
    vector<string>argsWOflags=stripFlags(args,cmd);
    if(argsWOflags.size()==0){
        ret_cmd=cmd;
        ret_args=args;
        return;
        // return make_tuple(cmd,args);
    }
    string nextSubCmd = argsWOflags[0];
    Command* next_cmd=cmd.findNext(nextSubCmd);
    if(next_cmd){
        vector<string>next_args =cmd.argsMinusFirstX(args, nextSubCmd);
        innerfind(*next_cmd,next_args,ret_cmd,ret_args);
        return;
    }
    ret_cmd=cmd;
    ret_args=args;
    return;
    // return make_tuple(cmd,args);
}
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
void Command::Find(vector<string>args,Command& ret_cmd,vector<string>&ret_args){
    // Command commandFound;
    // vector<string> new_args;
    innerfind(*this,args,ret_cmd,ret_args);
    // return make_tuple(a,commandFound,);
}
bool commandNameMatches(string s, string t){
    return s==t;
}
Command* Command::findNext(string next){
    // vector<Command*> matches;
    for (auto cmd:this->Son_command){
        if(commandNameMatches(cmd->name,next)){
            cmd->commandcallas.name=next;
            return cmd;
        }
    }
    return nullptr;
}
void Command::execute(vector<string> args){
    InitDefaultHelpFlag();
    InitDefaultVersionFlag();
    ParseFlags(args);
    bool helpval=Flags()->GetBool("help");
    if(helpval){
        return;
    }
    if(Runnable()){
        return;
    }
    // preRun();
    vector<string> argWoFlags = Flags()->Args();
    if(DisableFlagParsing){
        argWoFlags=args;
    }
    // vector<Command*> parents;
    // for (auto p=this;p!=nullptr;p=p->Parent()){
    //     parents.emplace_back(p);
    // }

    for(auto p=this;p!=nullptr;p=p->Parent()){
        if(p->PersistentPreRun!=nullptr){
            p->PersistentPreRun(*this,argWoFlags);
        }
    }
    if(PreRun!=nullptr){
        PreRun(*this,argWoFlags);
    }
    if(ValidateRequiredFlags()||ValidateFlagGroups()){
        return;
    }
    if(Run!=nullptr){
        Run(*this,argWoFlags);
    }
    if(PostRun!=nullptr){
        PostRun(*this,argWoFlags);
    }
    for (auto p=this;p!=nullptr;p=p->Parent()){
        if(p->PersistentPostRun!=nullptr){
            p->PersistentPostRun(*this,argWoFlags);
        }
    }
    return;
}
void Command::ParseFlags(vector<string> args){
    mergePersistentFlags();
    Flags()->Parse(args);
}
bool Command::ValidateRequiredFlags(){
    auto flags=Flags();
    vector<string> missingFlagNames;
    
    function<void(Flag*)> fn=[&missingFlagNames](Flag* f){

    };
    flags->VisitAll(fn);
    if(missingFlagNames.size()>0){
        cerr<<"required flag(s) "<<to_string(missingFlagNames.size()) <<" not set"<<endl;
    }
    return true;
}
bool Command::ValidateFlagGroups(){
    ///<暂时未定义
    return true;
}
bool Command:: Runnable(){
    if(Run!=nullptr){
        return true;
    }
    return false;
}
void Command::mergePersistentFlags(){
    updateParentsPflags();
    Flags()->AddFlagSet(PersistentFlags());
    Flags()->AddFlagSet(parent_persistent_flags);
}
void Command::updateParentsPflags(){
    parent_persistent_flags->SortedFlags=false;
    Root()->PersistentFlags()->AddFlagSet(&CommandLine);
    // Root()->PersistentFlags()->AddFlagSet(CommandLine);
    VisitParents();
}
void Command::VisitParents(){
    if (HasParent()){
        parent_persistent_flags->AddFlagSet(Parent()->PersistentFlags());
        Parent()->VisitParents();
    }
}