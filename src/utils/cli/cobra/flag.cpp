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

#include "utils/cli/cobra/flag.h"
#include "utils/cli/cli/common.h"
#include "utils/logger/ProcessSafeLogger.h"
/**
 * @brief 构造一个新的Flag::Flag对象
 * 
 * @param name 标签名
 * @param usage 标签的用途
 * @param v 标签的值
 * @param values 标签的默认值
 */
Flag::Flag(string name,string usage,shared_ptr<Value_interface> v,string values):name(name),usage_help(usage),value(v),default_value(values) {}

/**
 * @brief 将标签添加到标签集中
 * 
 * @param value 标签的值
 * @param name 标签的名
 * @param usage 标签的用途
 * @return std::shared_ptr<Flag> 返回标签的指针
 */
shared_ptr<Flag> Flagset::Addvar(shared_ptr<Value_interface> value,string name,string usage){
    auto flag=std::make_shared<Flag>(name,usage,value,value->String());
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
    auto pvalue=newStringValue(value,option_name);
    auto flag=Addvar(pvalue,name,usage);
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
    auto pvalue=newStringArrayValue(value,option_name);
    auto flag=Addvar(pvalue,name,usage);
}

/**
 * @brief 根据字符串构建字符串标志
 * <p>string 定义具有指定名称、默认值和用法字符串的字符串标志。
 * @param name 标签名
 * @param value 标签的值
 * @param usage 标签用途介绍
 */
void Flagset::String(string name, string value, string usage){
    string* option_name=new string();
    StringVar(*option_name,name,value,usage);
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
    auto pvalue=newBoolValue(value,option_name);
    auto flag=Addvar(pvalue,name,usage);
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
    auto pvalue=newIntValue(value,option_name);
    auto flag=Addvar(pvalue,name,usage);
}
/**
 * @brief 创建值为int64_t的标签
 * 
 * @param option_name 
 * @param name 
 * @param value 
 * @param usage 
 */
/**
 * @brief 创建值为int64_t的标签
 * 
 * @param option_name 存储标志值的int64_t变量引用
 * @param name 标签名
 * @param value 标签默认值
 * @param usage 标签用途说明
 */
void Flagset::Int64Var(int64_t& option_name, string name, int value, string usage){
    auto pvalue=newInt64Value(value,option_name);
    auto flag=Addvar(pvalue,name,usage);
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
    auto pvalue=newUint64Value(value,option_name);
    auto flag=Addvar(pvalue,name,usage);
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
    auto pvalue=newStringSliceValue(value,option_name);
    auto flag=Addvar(pvalue,name,usage);
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
        logger->log_error("no such flag: "+name);
        cerr<<"no such flag: "<<name<<endl;
    }
    flag->second->Annotations[key]=value;
}
/**
 * @brief AddFlagSet 将一个 FlagSet 添加到另一个 FlagSet
 * 
 * @param newflagset 需要添加的标签集
 */
void Flagset::AddFlagSet(std::shared_ptr<Flagset> newflagset){
    if(newflagset==nullptr){
        return;
    }
    newflagset->VisitAll([this](shared_ptr<Flag> f){
        this->Lookup(f->name);
        this->AddFlag(f);
    });
}
/**
 * @brief AddFlag 会将标志添加到 FlagSet
 * 
 * @param newflag 被添加的标签
 */
void Flagset::AddFlag(shared_ptr<Flag> newflag){
    string name= newflag->name;
    auto it =formal_flags.find(name);
    if(it!=formal_flags.end()){
        // cout<<this->name<<" flag "<<name<< " redefined: "<<name<<endl;
        return;
    }
    // if(formal_flags)
    formal_flags[name]=newflag;
    // *newf=*newflag;
    order_formal_flags.emplace_back(newflag);
}
/**
 * @brief 如果 f.SortFlags 为 false，则 VisitAll 按字典顺序或原始顺序访问标志，并为每个标志调用 fn。
 * <p>它访问所有标志，甚至是那些未设置的标志。
 * 
 * @param fn 可调用对象
 */
void Flagset::VisitAll(const function<void(shared_ptr<Flag>)>& fn){
    if(formal_flags.size()==0){
        return;
    }
    // if(SortedFlags){
    //     if(formal_flags.size()!=sorted_formal_flags.size()){
    //         sorted_formal_flags=sortFlags(formal_flags);
    //     }
    // }
    vector<shared_ptr<Flag>>newflags;
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
 * @return std::shared_ptr<Flag> 标签名对应的标签
 */
// std::shared_ptr<Flag> Flagset::Lookup(const string name){
//     std::string n=name;
//     auto it= formal_flags.find(n);
//     if(it!=formal_flags.end()){
//         return it->second;
//     }else{
//         return nullptr;
//     }
// }
shared_ptr<Flag> Flagset::Lookup(const string name) {
    if (formal_flags.count(name) > 0) { // 如果容器中存在该键
        return formal_flags[name]; // 这里会返回该键对应的值，不会插入新元素
    } else {
        return nullptr; // 如果没有找到，返回 nullptr
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
    auto flag=Lookup(name);
    if(flag==nullptr){
        logger->log_error("flag "+name+" does not exit");
        throw myerror("flag "+name+" does not exit");
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
    auto flag=Lookup(name);
    if(flag==nullptr){
        logger->log_error("flag accessed but not defined: "+name);
        throw myerror("flag accessed but not defined: "+name);
        // cerr<<"flag accessed but not defined: "<<name<<endl;
        return "";
    }
    if(flag->value->Type()!=ftype){
        logger->log_error("trying to get"+ ftype+" value of flag of type "+flag->value->Type());
        throw myerror("trying to get"+ ftype+" value of flag of type "+flag->value->Type());
        // cerr<<"trying to get"<< ftype<<" value of flag of type "<<flag->value->Type()<<endl;
        return "";
    }
    string sval=flag->value->String();
    // string ret=ParseBool(sval);
    return sval;

}

/**
 * @brief 获取bool类型的标志值
 * 
 * @param name 标签名
 * @return bool 标志的值
 * @throws myerror 标志未定义或类型不匹配时抛出异常
 */
bool Flagset::GetBool(string name) {
    try {
        // 获取标志的值，并将其转换为小写
        string val = getFlagType(name, "bool");
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);

        // 如果标志的值为“true”，则返回true，否则返回false
        return (val == "true");
    } catch (const myerror& e) {
        throw;
    }
}
/**
 * @brief 获取字符串对象
 * 
 * @param name 标签名
 * @return string 标志的值
 * @throws myerror 标志未定义或类型不匹配时抛出异常
 * 
 * @details 该函数通过调用getFlagType函数获取标志的值，并将其作为字符串返回。
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
 * @brief Args 返回非标志参数
 * <p>
 * Args 函数返回在分析完命令行参数后，剩下的非标志参数
 * </p>
 * @return vector<string> 非标志参数
 */
vector<string> Flagset::Args(){
    return args;
}
/**
 * @brief sortFlags 将标志作为按字典排序顺序的切片返回。
 * 
 * @param flags 被排序的标签集
 * @return vector<std::shared_ptr<Flag>> 已排序的标签集
 */
vector<shared_ptr<Flag>> sortFlags(map<string,shared_ptr<Flag>> flags){
    vector<pair<string,shared_ptr<Flag>>> vec_flags(flags.begin(),flags.end());
    sort(vec_flags.begin(),vec_flags.end(),[](pair<string,shared_ptr<Flag>>&s1,pair<string,shared_ptr<Flag>>&s2){
        return s1.first<s2.first;
    });
    vector<shared_ptr<Flag>> ret_flags;
    for (auto f:vec_flags){
        ret_flags.emplace_back(f.second);
    }
    return ret_flags;
}
/**
 * @brief Parse 从参数列表中解析标志定义
 * 
 * @param arguments 参数列表
 * 
 * @details 此函数用于解析传递的参数列表。首先，它将
 * 解析状态设置为 true。然后，它检查参数列表是否为空。
 * 如果为空，则直接返回。否则，将参数列表的大小保留
 * 在成员变量 args 中，并调用 parseArgs 函数进行解析。
 * 如果解析过程中出现错误，则捕获并重新抛出异常。
 */
void Flagset::Parse(vector<string> arguments) {
    // 设置解析状态为 true
    parsed = true;

    // 检查参数列表是否为空
    if (arguments.size() == 0) {
        return;
    }

    // 预留参数列表大小
    args.reserve(arguments.size());

    try {
        // 调用 parseArgs 函数进行参数解析
        parseArgs(arguments);
    } catch (const myerror& e) {
        // 捕获并重新抛出异常
        throw;
    }
}
/**
 * @brief 分析子命令参数
 * 
 * @param args 参数列表
 * 
 * @details 该函数用于解析子命令参数。首先，如果参数列表为空
 *          或者第一个参数不是长标志（以 "--" 开头），
 *          则将参数列表的所有元素添加到成员变量 args 中
 *          并返回。如果参数列表不为空且第一个参数是长
 *          标志，则将其解析并将解析结果添加到参数列表
 *          中，并继续解析下一个参数
 */
void Flagset::parseArgs(vector<string> args){
    auto it =args.begin();
    while (it!=args.end()){
        string str=*it;
        it=args.erase(it);
        /// 如果参数列表为空或者第一个参数不是长标志
        if(str.size()<2||str[0]!='-'||str.size()==1||str[1]!='-'){
            if(!interspersed){
                /// 如果不允许散布参数
                this->args.emplace_back(str);
                this->args.insert(this->args.end(),it,args.end());
                return;
            }
            /// 否则，将参数添加到参数列表
            this->args.emplace_back(str);
            continue;
        }
        vector<string> in_args(it,args.end());
        try
        {
            /// 解析长标志
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
 * @param arg 长标签名
 * @param args 输入参数列表
 * @return vector<string> 解析后的参数列表
 * 
 * @details 该函数将长标签名 arg 解析，并将解析结果
 *          添加到参数列表 ret_args 中，并返回 ret_args
 */
vector<string> Flagset::parseLongArg(string arg, vector<string> args) {
    vector<string> ret_args = args;
    // 提取标签名
    string name(arg.begin() + 2, arg.end());
    if (name.size() == 0 || name[0] == '-' || name[0] == '=') {
        logger->log_error("bad flag syntax: " + arg);
        throw myerror("bad flag syntax: " + arg);
    }

    // 分割标签名和值
    vector<string> split = SplitN(name, "=", 2);
    name = split[0];

    std::shared_ptr<Flag> flag=nullptr;
    if (formal_flags.find(name) == formal_flags.end()) {
        logger->log_error("unknown flag: --" + name);
        throw myerror("unknown flag: --" + name);
    } else {
        flag = formal_flags[name];
    }

    string value;
    if (split.size() == 2) {
        // 格式为 --flag=value
        value = split[1];
    } else if (flag->NoOptDefVal != "") {
        // 格式为 --flag, 参数为可选
        value = flag->NoOptDefVal;
    } else if (ret_args.size() > 0) {
        // 格式为 --flag arg
        value = ret_args[0];
        ret_args.erase(ret_args.begin());
    } else {
        // 格式为 --flag, 参数为必需
        logger->log_error("flag needs an argument: " + arg);
        throw myerror("flag needs an argument: " + arg);
    }

    try {
        // 尝试设置标志
        bool isSet = Set(flag->name, value);
        if (!isSet) {
            logger->log_error( "fail to set the flag value");
            cerr << "fail to set the flag value" << endl;
        }
    } catch (const myerror& e) {
        throw;
    }

    return ret_args;
}
/**
 * @brief Set 设置指定标志的值
 * 
 * @param name 标签名
 * @param value 标签值
 * 
 * @details 该函数用于将指定的标志设置为指定的值。
 *          如果标志不存在，或者标志的值类型不匹配
 *          则抛出异常。
 * @return true 如果标志设置成功
 * @return false 如果标志设置失败
 */
bool Flagset::Set(string name, string value){
    // string name =this->name;
    shared_ptr<Flag> flag=nullptr;
    if(formal_flags.find(name)==formal_flags.end()){
        logger->log_error("no such flag "+name);
        throw myerror("no such flag "+name);
        // cerr<<"no such flag "<<name<<endl;
        return false;
    }else{
        flag=formal_flags[name];
    }
    // Flag& flag=f;
    flag->value->Set(value);
    logger->set_module("flag");
    logger->log_info("flag: "+std::string(flag->name)+"\tvalue: "+flag->value->String());
    cout<<"flag: "<<flag->name<<"\tvalue: "<<flag->value->String()<<endl;
    if(!flag->changed){
        flag->changed=true;
        actual_flags[flag->name]=flag;
        order_actual_flags.emplace_back(flag);
    }
    if(flag->deprecated!=""){
        logger->log_error("flag "+name+"has been deprecated, "+flag->deprecated);
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
