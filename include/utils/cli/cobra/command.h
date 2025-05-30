/**
 * @file command.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对工具的命令基础类的成员变量和成员函数的声明
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef UTILS_CLI_COBRA_COMMAND_H
#define UTILS_CLI_COBRA_COMMAND_H

// #include <errors.h>
#include <numeric>
#include <tuple>
#include <memory>
// #include <ios>
#include <functional>
// #include "utils/cli/cobra/flag.h"
#include "utils/cli/cobra/flag.h"
// #include "context.h"
// using namespace std;
/**
 * @brief Command定义应用程序的命令。
 * @class Command
 * 例如'go run ...'，'run' 是命令。需要将用法和描述定义为命令的一部分
 * 以确保可用性。
 */
class Command: public std::enable_shared_from_this<Command> {
    public:
        /**
         * @brief commandCalledAs 是用于调用此命令的名称或别名值。
         * 
         */
        using CommandcalledAs= class {
                public:
                string name;
                bool called=false;
            };

        string name;                    ///<命令名称
        string Short;                   ///<命令简短的help介绍
        string Long;                    ///<命令详细的help介绍，用于help command输出
        string example;                 ///<使用该命令的用法例子
        // string category; ///<
        map<string,string> Annotations;
        string usageTemplate;           ///<useTemplate 是用户定义的使用模板。
        string helpTemplate;           ///<helpTemplate 是用户定义的帮助模板。

        string version;                 ///<命令版本
        vector<string> args;            ///<args 是从标志解析的实际参数。

        vector<std::shared_ptr<Command>> Son_command;   ///<该程序支持的子命令
        std::shared_ptr<Command> parent_Command=nullptr;        ///<parent 是该命令的父命令。
        std::shared_ptr<Command> helpCommand=nullptr;           ///<helpCommand 是使用“help”的命令。
        std::shared_ptr<Flagset> flags=nullptr;                  ///<flags 是全套完整标志。
        std::shared_ptr<Flagset> persistent_flags=nullptr;       ///<persistent_flags 包含持久标志。
        std::shared_ptr<Flagset> local_flags=nullptr;            ///<local_flags 包含本地标志。
        std::shared_ptr<Flagset>inherited_flags=nullptr;        ///<inherited_flags 包含继承的标志。
        std::shared_ptr<Flagset> parent_persistent_flags=nullptr;///<Parent_persistent_flags 是 cmd 父级的所有持久标志。
        CommandcalledAs commandcallas;           ///<commandcallas 是用于调用此命令的名称或别名值。
        std::function<bool(std::shared_ptr<Command>,vector<string>&)>Args; ///<检查命令的参数
        void (*helpFunc)(std::shared_ptr<Command> cmd, vector<string> strs)=nullptr; ///<help 函数
        bool TraverseChildren=false;            ///<TraverseChildren 在执行子命令之前解析所有父级上的标志。
        bool Hidden=false;                      ///<Hidden定义此命令是否隐藏并且不应显示在可用命令列表中。
        bool DisableFlagParsing=false;          ///<禁用标志解析​​，如果true，所有标志都将作为参数传递给命令。
        bool commandsAreSorted=false;           ///<CommandsAreSorted 定义命令切片是否已排序。
        int commandsMaxUseLen = 0;              ///<用于填充的命令字符串长度的最大长度。
        int commandsMaxCommandPathLen = 0;      ///<
        int commandsMaxNameLen = 0;             ///<
        /**
         * @defgroup 函数指针
         * @brief 声明命令运行入口函数
         * 函数函数指针指向函数，方便对不同命令的Run函数进行不同的定义
         * 
         * *Run 函数按以下顺序执行：
         * - PersistentPreRun()
         * - PreRun()
	     * - Run()
	     * - PostRun()
         * - PersistentPostRun()
         * <p>所有函数都具有相同的参数，即命令名称后面的参数。
         * @{
         * 
         */
        function <void(std::shared_ptr<Command> , vector<string>)> PersistentPreRun=nullptr;    ///<PersistentPreRun：该命令的子命令将继承并执行。
        function <void(std::shared_ptr<Command> , vector<string>)> PreRun=nullptr;              ///<PreRun：该命令的子命令不会继承。
        function <void(std::shared_ptr<Command> , vector<string>)> Run=nullptr;                 ///<Run：通常是实际功函数。大多数命令只会实现这个。
        function <void(std::shared_ptr<Command> , vector<string>)> PostRun=nullptr;             ///<PostRun：在运行命令之后运行。
        function <void(std::shared_ptr<Command> , vector<string>)> PersistentPostRun=nullptr;   ///<PersistentPostRun：此命令的子命令将在 PostRun 之后继承并执行。
        // void (*PreRun)(std::shared_ptr<Command> cmd, vector<string> args)=nullptr;              ///<PreRun：该命令的子命令不会继承。
        // void (*Run)(std::shared_ptr<Command> cmd, vector<string> args)=nullptr;                 ///<Run：通常是实际功函数。大多数命令只会实现这个。
        // void (*PostRun)(std::shared_ptr<Command> cmd, vector<string> args)=nullptr;             ///<PostRun：在运行命令之后运行。
        // void (*PersistentPostRun)(std::shared_ptr<Command> cmd, vector<string> args)=nullptr;   ///<PersistentPostRun：此命令的子命令将在 PostRun 之后继承并执行。
        /**
         * @}
         * 
         */
        //成员方法
        
        Command()=default; ///<默认的构造函数
        Command(string& name,string& Short,string& Long,string& example);  
        // ~Command();
        void Execute(int argc, char const *argv[]);
        void ExecuteC(int argc, char const *argv[]);
        void execute(vector<string> args);
        std::shared_ptr<Flagset> Flags(); 
        string  Name();
        std::shared_ptr<Flagset> PersistentFlags(); 
        void AddCommand(initializer_list<std::shared_ptr<Command>>cmdlist); 
        void RemoveCommand(initializer_list<std::shared_ptr<Command>>cmdlist);
        string CommandPath();
        void SetUsageTemplate(string&);
        void Help();
        void (*Helpfunc())(std::shared_ptr<Command> cmd, vector<string> str);
        bool HasParent();
        std::string HelpTemplate();
        std::shared_ptr<Command> Root();
        std::shared_ptr<Command> Parent();
        bool HasSubCommands();
        void InitDefaultHelpCmd();
        void InitDefaultCompletionCmd();
        void InitDefaultHelpFlag();
        void InitDefaultVersionFlag();
        void Traverse(vector<string>args,std::shared_ptr<Command> ret_cmd,vector<string>&ret_args);
        std::shared_ptr<Command> Find(vector<string>args,vector<string>&ret_args);
        std::shared_ptr<Command> findNext(string next);
        vector<string> argsMinusFirstX(vector<string>args,string x);
        void ParseFlags(vector<string> args);
        bool Runnable();
        // void preRun();
        // void postRun();
        bool ValidateRequiredFlags();
        bool ValidateFlagGroups();
        void mergePersistentFlags();
        void updateParentsPflags();
        void VisitParents(const function<void(std::shared_ptr<Command>)>& fn);
        std::shared_ptr<Flag> Flag_find(string name);
        std::shared_ptr<Flag> persistentFlag_find(string name);
        bool HasPersistentFlags();
        
};      

extern std::shared_ptr<Flagset> CommandLine;

std::shared_ptr<Flagset> NewFlagSet(const string& name);
void help_func(std::shared_ptr<Command> cmd, vector<string> a);
bool hasNoOptDefVal( string name, std::shared_ptr<Flagset> flags);
vector<string> stripFlags(vector<string> args,std::shared_ptr<Command> cmd);
std::shared_ptr<Command> innerfind(std::shared_ptr<Command> cmd,vector<string>&args,vector<string>& ret_args);
bool commandNameMatches(string s, string t);
void legacyArgs(std::shared_ptr<Command>cmd,vector<string>args);
#endif