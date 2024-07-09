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
// Flag::Flag(string name,string usage,Value* v,string values):name(name),usage_help(usage),value(std::make_unique<Value>(v)),default_value(values) {}

Flag::~Flag(){
    if(value){
        // cout << "Deleting value pointer: " << value << endl;
        delete value;
        value=nullptr;
    }
}

Flagset::~Flagset(){
    // 释放 actual_flags 中的 Flag 对象
    // for (auto& pair : actual_flags) {
    //     delete pair.second;
    // }
    // 释放 order_actual_flags 中的 Flag 对象
    // for (Flag* flag : order_actual_flags) {
    //     delete flag;
    // }
    // 释放 sorted_actual_flags 中的 Flag 对象
    // for (Flag* flag : sorted_actual_flags) {
    //     delete flag;
    // }
    // 释放 formal_flags 中的 Flag 对象
    // for (auto& pair : formal_flags) {
    //     delete pair.second;
    // }
    // 释放 order_formal_flags 中的 Flag 对象
    // for (Flag* flag : order_formal_flags) {
    //     if(flag)
    //     delete flag;
    // }
    // 释放 sorted_formal_flags 中的 Flag 对象
    // for (Flag* flag : sorted_formal_flags) {
    //     delete flag;
    // }
    // 不需要释放 output 指针，因为它不是由 Flagset 类分配的内存
    output = nullptr;
}
Flag* Flagset::Addvar(Value* value,string name,string usage){
    Flag* flag=new Flag{name,usage,value,value->String()};
    AddFlag(flag);
    // ret_flag=*flag;
    return flag;
}
/**
 * @brief 根据字符串变量构建字符串标志
 * 
 * StringVar定义一个具有指定名称、默认值和用法字符串的字符串标志。
 */
void Flagset::StringVar(string &option_name, string name,string value, string usage){
    StringValue* pvalue=newStringValue(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
    return ;
}

/**
 * @brief 根据字符数组构建字符串标志
 * 
 * StringArrayVar定义一个具有指定名称、默认值和用法字符串的字符串标志。
 */
void Flagset::StringArrayVar(vector<string>& option_name, string name ,vector<string> value , string usage){
    StringArrayValue* pvalue=newStringArrayValue(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
}

/**
 * @brief 根据字符串构建字符串标志
 * 
 * string 定义具有指定名称、默认值和用法字符串的字符串标志。
 */
void Flagset::String(string name, string value, string usage){
    string option_name;
    StringValue* pvalue=newStringValue(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
}

/**
 * @brief 根据bool标志进行构建
 * 
 * BoolVar 定义一个具有指定名称、默认值和用法字符串的布尔标志。
 */
void Flagset::BoolVar(bool& option_name, string name, bool value, string usage){
    BoolValue* pvalue=newBoolValue(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
    flag->NoOptDefVal="true";
}
void Flagset::BoolP(string name,bool value, string usage){
    bool option_name;
    BoolVar(option_name,name,value,usage);
}
void Flagset::IntVar(int& option_name, string name, int value, string usage){
    IntValue* pvalue=newIntValue(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
}

void Flagset::Int(string name, int value, string usage){
    int option_name;
    IntValue* pvalue=newIntValue(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
}

void Flagset::StringSliceVar(vector<string>& option_name, string name ,vector<string> value , string usage){
    stringSliceValue* pvalue=newStringSliceValue(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
}

/**
 * @brief 对简化的标志进行标准化
 * 
 * NormalizeFunc 允许您添加一个可以翻译标志名称的函数。
 * 添加到FlagSet的标志将被翻译，然后当任何东西试图查找也将被翻译的标志时。
 */
// void Flagset::NormalizeFunc(){
    
// } 

void Flagset::SetInterspersed(bool interspersed){
    this->interspersed=interspersed;
}
void Flagset::SetAnnotation(string name,string key,vector<string> value){
    auto flag=formal_flags.find(name);
    if(flag==formal_flags.end()){
        cerr<<"no such flag: "<<name<<endl;
    }
    flag->second->Annotations[key]=value;
}
void Flagset::AddFlagSet(Flagset* newflagset){
    if(newflagset==nullptr){
        cerr<<" new flag is nulpptr"<<endl;
    }
    // if(newflagset->formal_flags.size()==0){
    //     cerr<<"the new flagsets is null"<<endl;
    //     return;
    // }
    // vector<Flag*> newflags;
    // if(newflagset->SortedFlags){
    //     if(newflagset->formal_flags.size()!=newflagset->sorted_formal_flags.size()){
    //         newflagset->sorted_formal_flags=sortFlags(newflagset->formal_flags);
    //     }
    //     newflags=newflagset->sorted_formal_flags;
    // }else{
    //     newflags=newflagset->order_formal_flags;
    // }
    // for(auto flag:newflags){
    //     if(!Lookup(flag->name)){
    //         AddFlag(flag);
    //     }
    // }
    // Flagset* fs=this;
    // function<void(Flag*)>fn =[this](Flag* f){
    //     this->Lookup(f->name);
    //     this->AddFlag(f);
    // };
    // newflagset->VisitAll(fn)
    newflagset->VisitAll([this](Flag* f){
        this->Lookup(f->name);
        this->AddFlag(f);
    });
}
void Flagset::AddFlag(Flag* newflag){
    string name= newflag->name;
    auto it =formal_flags.find(name);
    if(it!=formal_flags.end()){
        cout<<this->name<<" flag "<<name<< " redefined: "<<name<<endl;
        return;
    }
    // if(formal_flags)
    formal_flags[name]=newflag;
    // Flag* newf=new Flag();
    // *newf=*newflag;
    order_formal_flags.emplace_back(newflag);
}
// void Visitflag(Flag& flag,vector<string>& names){

// }
void Flagset::VisitAll(const function<void(Flag*)>& fn){
    if(formal_flags.size()==0){
        cerr<<"the new flagsets is null"<<endl;
        return;
    }
    // if(SortedFlags){
    //     if(formal_flags.size()!=sorted_formal_flags.size()){
    //         sorted_formal_flags=sortFlags(formal_flags);
    //     }
    // }
    vector<Flag*>newflags;
    if(SortedFlags){
        if(formal_flags.size()!=sorted_formal_flags.size()){
            sorted_formal_flags=sortFlags(formal_flags);
        }
        newflags=sorted_formal_flags;
    }else{
        newflags=order_formal_flags;
    }
    for(auto flag:newflags){
        // if(Lookup(flag->name)==nullptr){
        //     AddFlag(flag);
        // }
        fn(flag);
    }
}
Flag* Flagset::Lookup(const string& name){
    auto it= formal_flags.find(name);
    if(it!=formal_flags.end()){
        return it->second;
    }else{
        return nullptr;
    }
}
void Flagset::MarkHidden(string name){
    Flag* flag=Lookup(name);
    if(flag==nullptr){
        cerr<<"flag "<<name<<"does not exit"<<endl;
    }else{
        flag->hidden=true;
    }
}

string Flagset::getFlagType(string name,const string ftype){
    Flag* flag=Lookup(name);
    if(flag==nullptr){
        cerr<<"flag accessed but not defined: "<<name<<endl;
        return "";
    }
    if(flag->value->Type()!=ftype){
        cerr<<"trying to get"<< ftype<<" value of flag of type "<<flag->value->Type()<<endl;
        return "";
    }
    string sval=flag->value->String();
    string ret=ParseBool(sval);
    return ret;

}
        bool Flagset:: GetBool(string name){
    if(getFlagType(name,"bool")=="bool"){
        return true;
    }else{
        return false;
    }
}
vector<string> Flagset::Args(){
    return args;
}
vector<Flag*> sortFlags(map<string,Flag*> flags){
    vector<pair<string,Flag*>> vec_flags(flags.begin(),flags.end());
    sort(vec_flags.begin(),vec_flags.end(),[](pair<string,Flag*>&s1,pair<string,Flag*>&s2){
        return s1.first<s2.first;
    });
    vector<Flag*> ret_flags;
    for (auto f:vec_flags){
        ret_flags.emplace_back(f.second);
    }
    return ret_flags;
}
void Flagset::Parse(vector<string>arguments){
    parsed=true;
    if(arguments.size()==0){
        return;
    }
    args.reserve(arguments.size());
    parseArgs(arguments);
}
void Flagset::parseArgs(vector<string> args){
    auto it =args.begin();
    while (it!=args.end()){
        string str=*it;
        it=args.erase(it);
        if(str.size()<2||str[0]!='-'||str.size()==1||str[1]!='-'){
            if(!interspersed){
                this->args.emplace_back(*it);
                this->args.insert(this->args.end(),it,args.end());
                return;
            }else{
                cerr<<"args are not standard"<<endl;
                return;
            }
        }
        vector<string> in_args(it,args.end());
        args=parseLongArg(str,in_args);
        it=args.begin();
    }
}
vector<string> Flagset::parseLongArg(string arg,vector<string> args){
    vector<string> ret_args=args;
    string name(arg.begin()+2,arg.end());
    if(name.size()==0|| name[0]=='-'||name[0]=='='){
        cerr<<"bad flag syntax: "<<name<<endl;
        return ret_args;
    }
    vector<string> split=SplitN(name,"=",2);
    name=split[0];
    Flag* flag;
    if(formal_flags.find(name)==formal_flags.end()){
        cerr<<"unknown flag: --"<<name<<endl;
    }else{
        flag=formal_flags[name];
    }
    string value;
    if(split.size()==2){
        ///--flag=value
        value=split[1];
    }else if(flag->NoOptDefVal!=""){
        ///'--flag' (arg was optional)
        value=flag->NoOptDefVal;
    }else if(ret_args.size()>0){
        /// '--flag arg'
        value=ret_args[0];
        ret_args.erase(ret_args.begin());
    }else{
        /// '--flag' (arg was required)
        cerr<<"flag needs an argument: "<<arg<<endl;
        return ret_args;
    }
    if(!Set(flag->name,value)){
        cerr<<"fail to set the flag value"<<endl;
    }
    return ret_args;
}
bool Flagset::Set(string name, string value){
    // string name =this->name;
    Flag* flag;
    if(formal_flags.find(name)==formal_flags.end()){
        cerr<<"no such flag "<<name<<endl;
        return false;
    }else{
        flag=formal_flags[name];
    }
    // Flag& flag=f;
    flag->value->Set(value);
    cout<<flag->value->String()<<endl;
    if(!flag->changed){
        flag->changed=true;
        // if(actual_flags.size()==0){
        //     actual_flags=new map<string,Flag*>();
        // }
        actual_flags[name]=flag;
        order_actual_flags.emplace_back(flag);
    }
    if(flag->deprecated!=""){
        cerr<< "flag "<<name<<"has been deprecated, "<<flag->deprecated<<endl;
    }
    return true;
}