/**
 * @file flag.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief flag.h头文件对应的源文件，对声明的方法进行定义
 * @version 0.1
 * @date 2024-06-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "flag.h"
Flag::Flag(string name,string usage,Value* v,string values):name(name),usage_help(usage),value(v),default_value(values) {}

void Flagset::Addvar(Value* value,string name,string usage){
    Flag flag{name,usage,value,value->String()};
    AddFlag(flag);
}
/**
 * @brief 根据字符串变量构建字符串标志
 * 
 * StringVar定义一个具有指定名称、默认值和用法字符串的字符串标志。
 */
void Flagset::StringVar(string &option_name, string name,string value, string usage){
    StringValue pvalue=newStringValue(value,option_name);
    Addvar(&pvalue,name,usage);
    return ;
}

/**
 * @brief 根据字符数组构建字符串标志
 * 
 * StringArrayVar定义一个具有指定名称、默认值和用法字符串的字符串标志。
 */
void Flagset::StringArrayVar(vector<string>& option_name, string name ,vector<string> value , string usage){
    StringArrayValue pvalue=newStringArrayValue(value,option_name);
    Addvar(&pvalue,name,usage);
}

/**
 * @brief 根据字符串构建字符串标志
 * 
 * string 定义具有指定名称、默认值和用法字符串的字符串标志。
 */
void Flagset::String(string name, string value, string usage){
    string option_name;
    StringValue pvalue=newStringValue(value,option_name);
    Addvar(&pvalue,name,usage);
}

/**
 * @brief 根据bool标志进行构建
 * 
 * BoolVar 定义一个具有指定名称、默认值和用法字符串的布尔标志。
 */
void Flagset::BoolVar(bool& option_name, string name, bool value, string usage){
    BoolValue pvalue=newBoolValue(value,option_name);
    Addvar(&pvalue,name,usage);
}

void Flagset::IntVar(int& option_name, string name, int value, string usage){
    IntValue pvalue=newIntValue(value,option_name);
    Addvar(&pvalue,name,usage);
}

void Flagset::Int(string name, int value, string usage){
    int option_name;
    IntValue pvalue=newIntValue(value,option_name);
    Addvar(&pvalue,name,usage);
}

void Flagset::StringSliceVar(vector<string>& option_name, string name ,vector<string> value , string usage){
    stringSliceValue pvalue=newStringSliceValue(value,option_name);
    Addvar(&pvalue,name,usage);
}

/**
 * @brief 对简化的标志进行标准化
 * 
 * NormalizeFunc 允许您添加一个可以翻译标志名称的函数。
 * 添加到FlagSet的标志将被翻译，然后当任何东西试图查找也将被翻译的标志时。
 */
void Flagset::NormalizeFunc(){
    
} 

void Flagset::SetInterspersed(bool interspersed){
    this->interspersed=interspersed;
}

void Flagset::AddFlagSet(Flagset& newflagset){
    if(newflagset.formal_flags.size()==0){
        cerr<<"the new flagsets is null"<<endl;
        return;
    }
    vector<Flag> newflags;
    if(newflagset.SortedFlags){
        if(newflagset.formal_flags.size()!=newflagset.sorted_formal_flags.size()){
            newflagset.sorted_formal_flags=sortFlags(newflagset.formal_flags);
        }
        newflags=newflagset.sorted_formal_flags;
    }else{
        newflags=newflagset.order_formal_flags;
    }
    for(auto flag:newflags){
        if(!Lookup(flag.name)){
            AddFlag(flag);
        }
    }

}
void Flagset::AddFlag(Flag&newflag){
    string name= newflag.name;
    formal_flags[name]=newflag;
    order_formal_flags.emplace_back(newflag);
}
bool Flagset::Lookup(string& name){
    if(formal_flags.find(name)!=formal_flags.end()){
        return true;
    }
    return false;
}

vector<Flag> sortFlags(map<string,Flag>& flags){
    vector<pair<string,Flag>> vec_flags(flags.begin(),flags.end());
    sort(vec_flags.begin(),vec_flags.end(),[](pair<string,Flag>&s1,pair<string,Flag>&s2){
        return s1.first<s2.first;
    });
    vector<Flag> ret_flags;
    for (auto f:vec_flags){
        ret_flags.emplace_back(f.second);
    }
    return ret_flags;
}