/**
 * @file flag.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @ingroup flag
 * @brief flag模块用来定义Flag类和Flagset类
 * <p> 
 * @version 0.1
 * @date 2024-06-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COBRA_FLAG_H
#define COBRA_FLAG_H
#include <functional>
#include <memory>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include "pflag/value.h"
#include "cobra/error.h"
using namespace std;

/**
 * @ingroup flag
 * @brief 声明Flag类（工具命令的标志量）的成员变量和成员函数
 * <p>标志代表标志的状态。
 */
class Flag{
  public:
    string  name;                   ///<在命令行中显示的名称
    // string  shorthand;           ///<单字母缩写标志
    string  usage_help;             ///<帮助信息
    Value*  value=nullptr;          ///<用来保存标签从参数列表中分析得到值
    // unique_ptr<Value> value;
    string  default_value;          ///<标签的默认值
    string  NoOptDefVal;            ///< 默认值（作为文本）；如果该标志位于命令行上且没有任何选项
    map<string,vector<string>> Annotations; ///< 由 bash 自动完成代码使用
    bool    hidden=false;                 ///<是否允许在帮助/使用文本中隐藏标志
    bool    changed=false;                ///<如果用户设置该值（或者保留默认值）
    string  deprecated;                   ///<如果此标志已弃用，则此字符串是新的或现在要使用的字符串
    string  shorthand_deprecated;         ///<如果不推荐使用此标志的简写，则此字符串是新的或现在要使用的字符串
    // set<string> values;                ///<设定值
    Flag()=default;
    Flag(string name,string usage,Value* v,string values);
    ~Flag();
};
/**
 * @brief Flagset 表示一组已定义的标志。
 * @class Flagset
 * 
 */
class Flagset{
  public:
    string name;                              ///<标志集的名字
    bool    parsed=false;                     ///<是否已经进行解析
    bool    SortedFlags=false;                ///<标志是否被排序（按照名称排序）
    bool    interspersed=false;               ///<允许散布选项/非选项参数
    map<string,Flag*> actual_flags;           ///<真实的标志集，以标志名为索引
    vector<Flag*>    order_actual_flags;      ///<有序的真实标志集
    vector<Flag*>    sorted_actual_flags;     ///<已排序的真实标志集
    map<string,Flag*> formal_flags;           ///<正式的标志集（类似进行标准化后的）
    vector<Flag*>    order_formal_flags;      ///<有序的正式标志集
    vector<Flag*>    sorted_formal_flags;     ///<已排序的正式标志集
    ostream*    output=nullptr;               ///<用于输出
    vector<string> args;                      ///<标志后的参数
    
    Flagset()=default;                      ///<构造函数
    ~Flagset();
    Flag* Addvar(Value* value,string name,string usage);
    void StringVar(string &option_name, string name,string value, string usage);
    void StringArrayVar(vector<string>& option_name, string name ,vector<string> value , string usage);
    void String(string name, string value, string usage);
    void BoolVar(bool& option_name, string name, bool value, string usage);
    void BoolP(string name,bool value, string usage);
    void IntVar(int& option_name, string name, int value, string usage);
    void Int64Var(int64_t& option_name, string name, int value, string usage);
    void Uint64Var(uint64_t& option_name, string name, int value, string usage);
    void Int(string name, int value, string usage);
    void StringSliceVar(vector<string>& option_name, string name ,vector<string> value , string usage);
    void StringSlice(string name ,vector<string> value , string usage);
    void AddFlagSet(Flagset* flags);
    // void NormalizeFunc();
    void SetInterspersed(bool interspersed);
    void SetAnnotation(string name,string key,vector<string> value);
    void AddFlag(Flag* newflag);
    // void Visitflag(Flag& flag,vector<string>& names);
    void VisitAll(const function<void(Flag*)>&fn);
    Flag* Lookup(const string& name);
    bool Changed(string name );
    void MarkHidden(string name);
    string getFlagType(string name,string ftype);
    bool GetBool(string name);
    string GetString(string name);
    vector<string>GetStringArray(string name);
    uint64_t GetUint64(string name);
    int64_t GetInt64(string name);
    vector<string> Args();
    void Parse(vector<string>);
    void parseArgs(vector<string> args);
    bool Set(string name, string value);
    vector<string> parseLongArg(string arg,vector<string> args);
    bool HasFlags();
};

vector<Flag*> sortFlags(map<string,Flag*> flags);


#endif