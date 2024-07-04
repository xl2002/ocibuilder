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
                bool called;
            };
        
        string name;                    ///<命令名称
        string Short;                   ///<命令简短的help介绍
        string Long;                    ///<命令详细的help介绍，用于help command输出
        string example;                 ///<使用该命令的用法例子
        // string category; ///<
        string usageTemplate;           ///<useTemplate 是用户定义的使用模板。
        string version;                 ///<命令版本
        vector<string> args;            ///<args 是从标志解析的实际参数。

        vector<Command*> Son_command;   ///<改程序支持的子命令
        Command* parent_Command;        ///<parent 是该命令的父命令。
        Command* helpCommand;           ///<helpCommand 是使用“help”的命令。

        Flagset flags;                  ///<flags 是全套完整标志。
        Flagset persistent_flags;       ///<persistent_flags 包含持久标志。
        Flagset local_flags;            ///<local_flags 包含本地标志。
        Flagset inherited_flags;        ///<inherited_flags 包含继承的标志。
        Flagset parent_persistent_flags;///<Parent_persistent_flags 是 cmd 父级的所有持久标志。
        CommandcalledAs commandcallas;  ///<commandcallas 是用于调用此命令的名称或别名值。
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
        void (*PreRun)(); ///<PreRun：该命令的子命令不会继承。
        void (*Run)(); ///<Run：通常是实际功函数。大多数命令只会实现这个。
        void (*PostRun)(); ///<PostRun：在运行命令之后运行。
        /**
         * @}
         * 
         */
        //成员方法
        Command()=default; ///<默认的构造函数
        Command(string& name,string& Short,string& Long,string& example); ///<Command类的列表构造函数
        function<bool(Command&,vector<string>&)>Args; ///<检查命令的参数
        Flagset& Flags(); ///<返回命令的标志集
        Flagset& PersistentFlags(); ///<返回命令的持久化标志集
        Command& AddCommand(initializer_list<Command>cmdlist); ///<向命令中添加子命令
        void SetUsageTemplate(string&);
};

#endif