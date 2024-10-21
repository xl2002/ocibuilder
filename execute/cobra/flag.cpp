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

#include "cobra/flag.h"
#include "cli/common.h"
/**
 * @brief 构造一个新的Flag::Flag对象
 * 
 * @param name 标签名
 * @param usage 标签的用途
 * @param v 标签的值
 * @param values 标签的默认值
 */
Flag::Flag(string name,string usage,Value_interface* v,string values):name(name),usage_help(usage),value(v),default_value(values) {}
// Flag::Flag(string name,string usage,Value_interface* v,string values):name(name),usage_help(usage),value(std::make_shared<Value_interface>(v)),default_value(values) {}
/**
 * @brief 销毁Flag::Flag对象
 * 
 */
Flag::~Flag(){
    if(value){
        // cout << "Deleting value pointer: " << value << endl;
        delete value;
        value=nullptr;
    }
}
/**
 * @brief 销毁Flagset::Flagset对象
 * 
 */
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
    for (Flag* flag : sorted_actual_flags) {
        delete flag;
    }
    // 释放 formal_flags 中的 Flag 对象
    for (auto& pair : formal_flags) {
        delete pair.second;
    }
    // 释放 order_formal_flags 中的 Flag 对象
    // for (Flag* flag : order_formal_flags) {
    //     if(flag)
    //     delete flag;
    // }
    // 释放 sorted_formal_flags 中的 Flag 对象
    for (Flag* flag : sorted_formal_flags) {
        delete flag;
    }
    // 不需要释放 output 指针，因为它不是由 Flagset 类分配的内存
    output = nullptr;
}
/**
 * @brief 将标签添加到标签集中
 * 
 * @param value 标签的值
 * @param name 标签的名
 * @param usage 标签的用途
 * @return Flag* 返回标签的指针
 */
Flag* Flagset::Addvar(Value_interface* value,string name,string usage){
    Flag* flag=new Flag{name,usage,value,value->String()};
    AddFlag(flag);
    // ret_flag=*flag;
    return flag;
}

/**
 * @brief 根据字符串变量构建字符串标志
 * <p>StringVar定义一个具有指定名称、默认值和用法字符串的字符串标志。
 * @param option_name 可选项的名称
 * @param name 标签名
 * @param value 标签值
 * @param usage 标签用途介绍
 */
void Flagset::StringVar(string &option_name, string name,string value, string usage){
    StringValue* pvalue=newStringValue(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
    return ;
}

/**
 * @brief 根据字符数组构建字符串标志
 * <p> StringArrayVar定义一个具有指定名称、默认值和用法字符串的字符串标志。
 * @param option_name 可选项的名称
 * @param name 标签名
 * @param value 标签值
 * @param usage 标签用途介绍
 */
void Flagset::StringArrayVar(vector<string>& option_name, string name ,vector<string> value , string usage){
    StringArrayValue* pvalue=newStringArrayValue(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
}

/**
 * @brief 根据字符串构建字符串标志
 * <p>string 定义具有指定名称、默认值和用法字符串的字符串标志。
 * @param name 标签名
 * @param value 标签的值
 * @param usage 标签用途介绍
 */
void Flagset::String(string name, string value, string usage){
    string option_name;
    StringValue* pvalue=newStringValue(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
}

/**
 * @brief 根据bool标志进行构建
 * <p>BoolVar 定义一个具有指定名称、默认值和用法字符串的布尔标志。
 * @param option_name 可选项的名称
 * @param name 标签名
 * @param value 标签值
 * @param usage 标签用途介绍
 */
void Flagset::BoolVar(bool& option_name, string name, bool value, string usage){
    BoolValue* pvalue=newBoolValue(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
    flag->NoOptDefVal="true";
}
/**
 * @brief 根据标签名创建,没有给出标签值存储位置
 * 
 * @param name 标签名
 * @param value 标签值
 * @param usage 标签用途
 */
void Flagset::BoolP(string name,bool value, string usage){
    bool* option_name=new bool();
    BoolVar(*option_name,name,value,usage);
}
/**
 * @brief 创建值为int的标签
 * 
 * @param option_name int值保存的位置
 * @param name 标签名
 * @param value 标签值
 * @param usage 标签用途
 */
void Flagset::IntVar(int& option_name, string name, int value, string usage){
    IntValue* pvalue=newIntValue(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
}
/**
 * @brief 创建值为int64_t的标签
 * 
 * @param option_name 
 * @param name 
 * @param value 
 * @param usage 
 */
void Flagset::Int64Var(int64_t& option_name, string name, int value, string usage){
    int64Value* pvalue=newInt64Value(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
}
/**
 * @brief 创建值为uint64_t的标签
 * 
 * @param option_name 
 * @param name 
 * @param value 
 * @param usage 
 */
void Flagset::Uint64Var(uint64_t& option_name, string name, int value, string usage){
    uint64Value* pvalue=newUint64Value(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
}
/**
 * @brief 创建值为int的标签，没有提供存储位置
 * 
 * @param name 标签名
 * @param value 标签值
 * @param usage 标签用途
 */
void Flagset::Int(string name, int value, string usage){
    int* option_name=new int();
    // IntValue* pvalue=newIntValue(value,option_name);
    // Flag* flag=Addvar(pvalue,name,usage);
    IntVar(*option_name,name,value,usage);
}
/**
 * @brief StringSliceVar 定义一个具有指定名称、默认值和用法字符串的字符串标志。
 * 
 * @param option_name 字符串值保存的位置
 * @param name 标签名
 * @param value 标签值
 * @param usage 标签用途
 */
void Flagset::StringSliceVar(vector<string>& option_name, string name ,vector<string> value , string usage){
    stringSliceValue* pvalue=newStringSliceValue(value,option_name);
    Flag* flag=Addvar(pvalue,name,usage);
}
/**
 * @brief 字符串值保存的位置
 * 
 * @param name 
 * @param value 
 * @param usage 
 */
void Flagset::StringSlice(string name ,vector<string> value , string usage){
    vector<string>* option_name=new vector<string>();
    StringSliceVar(*option_name,name,value,usage);
}
/**
 * @brief SetInterspersed 设置是否支持散布选项/非选项参数。
 * 
 * @param interspersed 
 */
void Flagset::SetInterspersed(bool interspersed){
    this->interspersed=interspersed;
}
/**
 * @brief SetAnnotation 允许在 FlagSet 中的标志上设置任意注释。
 * 
 * @param name 标签名
 * @param key 注释关键字
 * @param value 注释内容
 */
void Flagset::SetAnnotation(string name,string key,vector<string> value){
    auto flag=formal_flags.find(name);
    if(flag==formal_flags.end()){
        // throw myerror("no such flag: "+name);
        cerr<<"no such flag: "<<name<<endl;
    }
    flag->second->Annotations[key]=value;
}
/**
 * @brief AddFlagSet 将一个 FlagSet 添加到另一个 FlagSet
 * 
 * @param newflagset 需要添加的标签集
 */
void Flagset::AddFlagSet(Flagset* newflagset){
    if(newflagset==nullptr){
        return;
        // cerr<<" new flag is nulpptr"<<endl;
    }
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
/**
 * @brief AddFlag 会将标志添加到 FlagSet
 * 
 * @param newflag 被添加的标签
 */
void Flagset::AddFlag(Flag* newflag){
    string name= newflag->name;
    auto it =formal_flags.find(name);
    if(it!=formal_flags.end()){
        // cout<<this->name<<" flag "<<name<< " redefined: "<<name<<endl;
        return;
    }
    // if(formal_flags)
    formal_flags[name]=newflag;
    // Flag* newf=new Flag();
    // *newf=*newflag;
    order_formal_flags.emplace_back(newflag);
}
/**
 * @brief 如果 f.SortFlags 为 false，则 VisitAll 按字典顺序或原始顺序访问标志，并为每个标志调用 fn。
 * <p>它访问所有标志，甚至是那些未设置的标志。
 * 
 * @param fn 可调用对象
 */
void Flagset::VisitAll(const function<void(Flag*)>& fn){
    if(formal_flags.size()==0){
        // cerr<<"the new flagsets is null"<<endl;
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
/**
 * @brief Lookup 返回指定标志的 Flag 结构，如果不存在则返回 nullptr。
 * 
 * @param name 标签名
 * @return Flag* 标签名对应的标签
 */
Flag* Flagset::Lookup(const string& name){
    auto it= formal_flags.find(name);
    if(it!=formal_flags.end()){
        return it->second;
    }else{
        return nullptr;
    }
}
/**
 * @brief 判断指定的标志是否发生了变化
 * @param name 标志的名称
 * @return true 如果标志发生了变化； false 如果标志未发生变化
 */
bool Flagset:: Changed(string name ) {
	auto flag = Lookup(name);
	// If a flag doesn't exist, it wasn't changed....
	if (flag == nullptr) {
		return false;
	}
	return flag->changed;
}
/**
 * @brief MarkHidden 在您的程序中设置一个“隐藏”标志。
 * <p>它将继续运行，但不会显示在帮助或使用消息中。
 * 
 * @param name 标签名
 */
void Flagset::MarkHidden(string name){
    Flag* flag=Lookup(name);
    if(flag==nullptr){
        throw myerror("flag "+name+"does not exit");
        // cerr<<"flag "<<name<<"does not exit"<<endl;
    }else{
        flag->hidden=true;
    }
}
/**
 * @brief 函数返回给定标志名称的给定类型
 * 
 * @param name 标签名
 * @param ftype 指定类型
 * @return string 
 */
string Flagset::getFlagType(string name,const string ftype){
    Flag* flag=Lookup(name);
    if(flag==nullptr){
        throw myerror("flag accessed but not defined: "+name);
        // cerr<<"flag accessed but not defined: "<<name<<endl;
        return "";
    }
    if(flag->value->Type()!=ftype){
        throw myerror("trying to get"+ ftype+" value of flag of type "+flag->value->Type());
        // cerr<<"trying to get"<< ftype<<" value of flag of type "<<flag->value->Type()<<endl;
        return "";
    }
    string sval=flag->value->String();
    // string ret=ParseBool(sval);
    return sval;

}
/**
 * @brief 获取布尔对象
 * 
 * @param name 标签名
 * @return true 如果标志的值为“true”，则返回true
 * @return false 如果标志的值不为“true”，则返回false
 */
bool Flagset::GetBool(string name){
    try
    {
        // 获取标志的值，并将其转换为小写
        string val = getFlagType(name, "bool");
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        
        // 如果标志的值为“true”，则返回true，否则返回false
        return (val == "true");
    }
    catch(const myerror& e)
    {
        throw;
    }
}
/**
 * @brief 获取字符串对象
 * 
 * @param name 标签名
 * @return 标志的值
 * @throws myerror 标志未定义或类型不匹配时抛出异常
 */
string Flagset::GetString(string name) {
    try {
        string val = getFlagType(name, "string");
        return val;
    } catch (const myerror& e) {
        throw;
    }
}
/**
 * @brief 将字符串按空格分割成字符串数组
 * 
 * @param input 输入字符串
 * @return std::vector<std::string> 分割后的字符串数组
 * @throws None 不会抛出异常
 */
std::vector<std::string> SplitStringBySpaces(const std::string& input) {
    std::vector<std::string> result;
    std::istringstream iss(input);
    std::string word;

    while (iss >> word) {
        result.push_back(word);
    }

    return result;
}
/**
 * @brief 获取字符串数组对象
 * 
 * @param name 标签名
 * @return std::vector<std::string> 标志的值
 * @throws myerror 标志未定义或类型不匹配时抛出异常
 * 
 * @details 该函数通过调用getFlagType函数获取标志的值，并将其按空格分割成字符串数组。
 * 分割函数为SplitStringBySpaces。
 */
vector<string> Flagset::GetStringArray(string name){
    try
    {
        string val=getFlagType(name,"stringArray");
        auto sval=val.substr(1,val.size()-2);
        return SplitStringBySpaces(sval);
    }
    catch(const myerror& e)
    {
        throw;
    }
}
/**
 * @brief 获取字符串片段对象
 * 
 * @param name 标签名
 * @return std::vector<std::string> 标志的值
 * @throws myerror 标志未定义或类型不匹配时抛出异常
 * 
 * @details 该函数通过调用getFlagType函数获取标志的值，并将其按空格分割成字符串数组。
 * 分割函数为SplitStringBySpaces。
 */
vector<string> Flagset::GetStringSlice(string name){
    try
    {
        string val=getFlagType(name,"stringSlice");
        auto sval=val.substr(1,val.size()-2);
        return SplitStringBySpaces(sval);
    }
    catch(const myerror& e)
    {
        throw;
    }
}
/**
 * @brief 获取uint64_t类型的标志值
 * 
 * @param name 标志名
 * @return uint64_t 标志值
 * @throws myerror 标志未定义或类型不匹配时抛出异常
 */
uint64_t Flagset::GetUint64(string name){
    try
    {
        string val=getFlagType(name,"uint64");
        return std::stoull(val);
    }
    catch(const myerror& e)
    {
        throw;
    }
}
int64_t Flagset::GetInt64(string name){
    try
    {
        string val=getFlagType(name,"int64");
        return std::stoll(val);
    }
    catch(const myerror& e)
    {
        throw;
    }
}
/**
 * @brief Args 返回非标志参数。
 * 
 * @return vector<string> 
 */

vector<string> Flagset::Args(){
    return args;
}
/**
 * @brief sortFlags 将标志作为按字典排序顺序的切片返回。
 * 
 * @param flags 被排序的标签集
 * @return vector<Flag*> 已排序的标签集
 */
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
/**
 * @brief Parse 从参数列表中解析标志定义
 * 
 * @param arguments 参数列表
 */
void Flagset::Parse(vector<string> arguments){
    parsed=true;
    if(arguments.size()==0){
        return;
    }
    args.reserve(arguments.size());
    try
    {
        parseArgs(arguments);
    }
    catch(const myerror& e)
    {
        throw;
    }
    
    
}
/**
 * @brief 分析子命令参数
 * 
 * @param args 参数列表
 */
void Flagset::parseArgs(vector<string> args){
    auto it =args.begin();
    while (it!=args.end()){
        string str=*it;
        it=args.erase(it);
        if(str.size()<2||str[0]!='-'||str.size()==1||str[1]!='-'){
            if(!interspersed){
                this->args.emplace_back(str);
                this->args.insert(this->args.end(),it,args.end());
                return;
            }else{
                cerr<<"args are not standard"<<endl;
                return;
            }
        }
        vector<string> in_args(it,args.end());
        try
        {
            args=parseLongArg(str,in_args);
        }
        catch(const myerror& e)
        {
            throw;
        }
        it=args.begin();
    }
}
/**
 * @brief 解析长标签名的参数
 * 
 * @param arg 
 * @param args 
 * @return vector<string> 
 */
vector<string> Flagset::parseLongArg(string arg,vector<string> args){
    vector<string> ret_args=args;
    string name(arg.begin()+2,arg.end());
    if(name.size()==0|| name[0]=='-'||name[0]=='='){
        throw myerror("bad flag syntax: "+arg);
        // cerr<<"bad flag syntax: "<<name<<endl;
        return ret_args;
    }
    vector<string> split=SplitN(name,"=",2);
    name=split[0];
    Flag* flag;
    if(formal_flags.find(name)==formal_flags.end()){
        throw myerror("unknown flag: --"+name);
        // cerr<<"unknown flag: --"<<name<<endl;
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
        throw myerror("flag needs an argument: "+arg);
        // cerr<<"flag needs an argument: "<<arg<<endl;
        // return ret_args;
    }
    // if(!Set(flag->name,value)){
    //     throw myerror("fail to set the flag value");
    //     // cerr<<"fail to set the flag value"<<endl;
    // }
    try
    {
        if(!Set(flag->name,value)){
            // throw myerror("fail to set the flag value");
            cerr<<"fail to set the flag value"<<endl;
        }
    }
    catch(const myerror& e)
    {
        throw;
    }
    
    return ret_args;
}/**
 * @brief Set 设置指定标志的值。
 * 
 * @param name 标签名
 * @param value 标签值
 * @return true 
 * @return false 
 */
bool Flagset::Set(string name, string value){
    // string name =this->name;
    Flag* flag;
    if(formal_flags.find(name)==formal_flags.end()){
        throw myerror("no such flag "+name);
        // cerr<<"no such flag "<<name<<endl;
        return false;
    }else{
        flag=formal_flags[name];
    }
    // Flag& flag=f;
    flag->value->Set(value);
    cout<<"flag: "<<flag->name<<"\tvalue: "<<flag->value->String()<<endl;
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
/**
 * @brief HasFlags 返回一个布尔值来指示 FlagSet 是否定义了任何标志。
 * 
 * @return true 
 * @return false 
 */
bool Flagset::HasFlags(){
    return formal_flags.size()>0;
}