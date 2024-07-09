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
#ifndef COMMAND_H
#define COMMAND_H

// #include <errors.h>
#include <numeric>
#include <tuple>
#include <ios>
#include <functional>
#include "flag.h"
// #include "context.h"
// using namespace std;
/**
 * @brief Command定义应用程序的命令。
 * @class Command
 * 例如'go run ...'，'run' 是命令。需要将用法和描述定义为命令的一部分
 * 以确保可用性。
 */
class Command{
    public:
        /**
         * @brief commandCalledAs 是用于调用此命令的名称或别名值。
         * 
         */
        using CommandcalledAs= struct {
                string name;
                bool called=false;
            };
        bool TraverseChildren=false;
        bool Hidden=false;
        bool DisableFlagParsing=false;
        bool commandsAreSorted=false;
        int commandsMaxUseLen = 0;
        int commandsMaxCommandPathLen = 0;
        int commandsMaxNameLen = 0;
        string name;                    ///<命令名称
        string Short;                   ///<命令简短的help介绍
        string Long;                    ///<命令详细的help介绍，用于help command输出
        string example;                 ///<使用该命令的用法例子
        // string category; ///<
        map<string,string> Annotations;
        string usageTemplate;           ///<useTemplate 是用户定义的使用模板。
        string version;                 ///<命令版本
        vector<string> args;            ///<args 是从标志解析的实际参数。

        vector<Command*> Son_command;   ///<该程序支持的子命令
        Command* parent_Command=nullptr;        ///<parent 是该命令的父命令。
        Command* helpCommand=nullptr;           ///<helpCommand 是使用“help”的命令。

        Flagset* flags=nullptr;                  ///<flags 是全套完整标志。
        Flagset* persistent_flags=nullptr;       ///<persistent_flags 包含持久标志。
        Flagset* local_flags=nullptr;            ///<local_flags 包含本地标志。
        Flagset* inherited_flags=nullptr;        ///<inherited_flags 包含继承的标志。
        Flagset* parent_persistent_flags=nullptr;///<Parent_persistent_flags 是 cmd 父级的所有持久标志。
// }
        CommandcalledAs commandcallas;  ///<commandcallas 是用于调用此命令的名称或别名值。
        function<bool(Command&,vector<string>&)>Args; ///<检查命令的参数
        void (*helpFunc)(Command& cmd, vector<string> strs)=nullptr;
        /**
         * @defgroup 函数指针
         * @brief 声明命令运行入口函数
         * 函数函数指针指向函数，方便对不同命令的Run函数进行不同的定义
         * 
         * *Run 函数按以下顺序执行：
         * - PreRun()
	     * - Run()
	     * - PostRun()
         * <p>所有函数都具有相同的参数，即命令名称后面的参数。
         * @{
         * 
         */
        void (*PersistentPreRun)(Command& cmd, vector<string>& args)=nullptr;
        void (*PreRun)(Command& cmd, vector<string>& args)=nullptr; ///<PreRun：该命令的子命令不会继承。
        void (*Run)(Command& cmd, vector<string>& args)=nullptr; ///<Run：通常是实际功函数。大多数命令只会实现这个。
        void (*PostRun)(Command& cmd, vector<string>& args)=nullptr; ///<PostRun：在运行命令之后运行。
        void (*PersistentPostRun)(Command& cmd, vector<string>& args)=nullptr;
        /**
         * @}
         * 
         */
        //成员方法
        Command()=default; ///<默认的构造函数
        Command(string& name,string& Short,string& Long,string& example); ///<Command类的列表构造函数
        ~Command();
        void Execute(int argc, char const *argv[]);
        void ExecuteC(int argc, char const *argv[]);
        void execute(vector<string> args);
        Flagset* Flags(); ///<返回命令的标志集
        string  Name();
        Flagset* PersistentFlags(); ///<返回命令的持久化标志集
        void AddCommand(initializer_list<Command*>cmdlist); ///<向命令中添加子命令
        void RemoveCommand(initializer_list<Command*>cmdlist);
        string CommandPath();
        void SetUsageTemplate(string&);
        
        void Help();
        void (*Helpfunc())(Command& cmd, vector<string> str);
        bool HasParent();
        Command* Root();
        Command* Parent();
        bool HasSubCommands();
        void InitDefaultHelpCmd();
        void InitDefaultCompletionCmd();
        void InitDefaultHelpFlag();
        void InitDefaultVersionFlag();
        void Traverse(vector<string>args,Command& ret_cmd,vector<string>&ret_args);
        void Find(vector<string>args,Command& ret_cmd,vector<string>&ret_args);
        Command* findNext(string next);
        vector<string> argsMinusFirstX(vector<string>args,string x);
        void ParseFlags(vector<string> args);
        bool Runnable();
        void preRun();
        void postRun();
        bool ValidateRequiredFlags();
        bool ValidateFlagGroups();
        void mergePersistentFlags();
        void updateParentsPflags();
        void VisitParents();
};      

extern Flagset CommandLine;

Flagset* NewFlagSet(const string& name);
void help_func(Command& cmd, vector<string> a);
bool hasNoOptDefVal( string name, Flagset* flags);
vector<string> stripFlags(vector<string> args,Command& cmd);
void innerfind(Command& cmd,vector<string>&args,Command& ret_cmd,vector<string>& ret_args);

#endif