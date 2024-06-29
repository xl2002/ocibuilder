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
#ifndef FLAG_H
#define FLAG_H
#include <string>
#include <set>
#include <map>
#include <vector>
#include <iostream>
using namespace std;

/**
 * @ingroup flag
 * @brief 声明Flag类（工具命令的标志量）的成员变量和成员函数
 * <p>标志代表标志的状态。
 */
class Flag{
  public:
    string  name;                   ///<在命令行中显示的名称
    string  shorthand;              ///<单字母缩写标志
    string  usage_help;             ///<帮助信息
    string  default_value;          ///<默认值
    bool    changed;                ///<如果用户设置该值（或者保留默认值）
    string  deprecated;             ///<如果此标志已弃用，则此字符串是新的或现在要使用的字符串
    string  shorthand_deprecated;   ///<如果不推荐使用此标志的简写，则此字符串是新的或现在要使用的字符串
    set<string> values;             ///<设定值
    
};
/**
 * @brief Flagset 表示一组已定义的标志。
 * @class Flagset
 * 
 */
class Flagset{
  public:
    string name;                            ///<标志集的名字
    bool    parsed;                         ///<是否已经进行解析
    bool    SortedFlags;                    ///<标志是否被排序（按照名称排序）
    map<string,Flag> actual_flags;          ///<真实的标志集，以标志名为索引
    vector<Flag>    order_actual_flags;     ///<有序的真实标志集
    vector<Flag>    sorted_actual_flags;    ///<已排序的真实标志集
    map<string,Flag> formal_flags;          ///<正式的标志集（类似进行标准化后的）
    vector<Flag>    order_formal_flags;     ///<有序的正式标志集
    vector<Flag>    sorted_formal_flags;    ///<已排序的正式标志集
    ostream*    output;                     ///<用于输出
    vector<string> args;                    ///<标志后的参数

    
    Flagset()=default;                      ///<构造函数
    void StringVar();                       ///<根据字符串变量构建字符串标志
    void StringArrayVar();                  ///<根据字符数组构建字符串标志
    void String();                          ///<根据字符串构建字符串标志
    void BoolVar();                         ///<根据bool标志进行构建
    void AddFlagSet(Flagset& flags);        ///<将构建好的标志集添加到命令结构中
    void NormalizeFunc();                   ///<对简化的标志进行标准化
    
};


#endif