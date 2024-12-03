/**
 * @file value.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 value.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-07-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "utils/cli/cobra/value.h"
/**
 * @brief 返回StringValue的字符串值
 * 
 * @return string 
 */
string StringValue::String(){
    return *value;
}
/**
 * @brief 设置StringValue的值
 * 
 * @param value 
 */
void StringValue::Set(string value){
    // *(this->value)=value;
    this->value=new string(value);
}
/**
 * @brief 返回StringValue的类型
 * 
 * @return string 
 */
string StringValue::Type(){
    return "string";
}
/**
 * @brief 新建一个StringValue类型
 * 
 * @param value StringValue类型的值
 * @param option_name 值保存的位置
 * @return StringValue* 
 */
StringValue* newStringValue(string value, string &option_name){
    option_name=value;
    StringValue* var=new StringValue(&option_name);
    return var;
}
/**
 * @brief 返回StringArrayValue的字符串值
 * 
 * @return string 
 */
string StringArrayValue::String(){
    std::ostringstream strout;
    if(!(*value).empty()){
        for(auto it=0;it<(*value).size()-1;it++){
            strout<<(*value)[it]<<" ";
        }
        strout<<(*value)[(*value).size()-1];
        return '['+strout.str()+']';
    }else{
        return "[]";
    }
    
}
/**
 * @brief 设置StringArrayValue类型的值
 * 
 * @param value 需要设置的值
 */
void StringArrayValue::Set(string value){
    if(!changed){
        vector<string>newvalue{value};
        *(this->value)=newvalue;
        changed=true;
    }else{
        this->value->emplace_back(value);
    }
}
/**
 * @brief 返回StringArrayValue的类型
 * 
 * @return string 
 */
string StringArrayValue::Type(){
    return "stringArray";
}
/**
 * @brief 在StringArrayValue类型中添加值
 * 
 * @param val 被添加的字符串
 */
void StringArrayValue::Append(string val){
    value->emplace_back(val);
}
/**
 * @brief 将StringArrayValue类型的值替换
 * 
 * @param newval 新的字符串数组
 */
void StringArrayValue::Repalce(vector<string> newval){
    *value=newval;
}
/**
 * @brief 返回StringArrayValue类型的字符串数组
 * 
 * @return vector<string> 
 */
vector<string> StringArrayValue::GetSlice(){
    return *value;
}
/**
 * @brief 新建StringArrayValue类型
 * 
 * @param value StringArrayValue类型的值
 * @param option_name 值存储的位置
 * @return StringArrayValue* 
 */
StringArrayValue* newStringArrayValue(vector<string> value,vector<string>& option_name){
    option_name=value;
    StringArrayValue* newvar=new StringArrayValue(&option_name);
    return newvar;
}
/**
 * @brief 返回BoolValue的字符串值
 * 
 * @return string 
 */
string BoolValue::String(){
    if(*value){
        return "true";
    }else{
        return "false";
    }
}
/**
 * @brief 设置BoolValue类型的值
 * 
 * @param value 
 */
void BoolValue::Set(string value){
    if(value=="true"){
        *(this->value)=true;
    }
    if(value=="false"){
        *(this->value)=false;
    }
}
/**
 * @brief 返回BoolValue的类型
 * 
 * @return string 
 */
string BoolValue::Type(){
    return "bool";
}
/**
 * @brief 检查BoolValue是否为bool类型的标签
 * 
 * @return true 
 * @return false 
 */
bool BoolValue::IsBoolFlag(){
    return true;
}
/**
 * @brief 新建BoolValue类型的对象
 * 
 * @param value 
 * @param option_name 
 * @return BoolValue* 
 */
BoolValue* newBoolValue(bool value,bool& option_name){
    option_name=value;
    BoolValue* new_val=new BoolValue(&option_name);
    return new_val;
}
/**
 * @brief 返回IntValue的字符串值
 * 
 * @return string 
 */
string IntValue::String(){
    return std::to_string(*value);
}
/**
 * @brief 设置IntValue类型的值
 * 
 * @param value 
 */
void IntValue::Set(string value){
    *(this->value)=std::stoi(value);
}
/**
 * @brief 返回IntValue的类型
 * 
 * @return string 
 */
string IntValue::Type(){
    return "int";
}
/**
 * @brief 新建IntValue类型
 * 
 * @param value IntValue类型的值
 * @param option_name 值存储的位置
 * @return IntValue* 
 */
IntValue* newIntValue(int value,int& option_name){
    option_name= value;
    IntValue* new_val=new IntValue(&option_name);
    return new_val;
}
/**
 * @brief 返回int64Value的字符串值
 * 
 * @return string 
 */
string int64Value:: String(){
    std::stringstream ss;
    ss<<std::hex<<(*value);
    return ss.str();
    // return std::to_string(*value);
}
/**
 * @brief 设置int64Value类型的值
 * 
 * @param value 
 */
void int64Value:: Set(string value){
    int64_t v=std::stoll(value);
   *(this->value)=v;
}
/**
 * @brief 返回int64Value的类型
 * 
 * @return string 
 */
string int64Value:: Type(){
    return "int64";
}
/**
 * @brief 新建Int64Value类型
 * 
 * @param value 
 * @param option_name 
 * @return int64Value* 
 */
int64Value* newInt64Value(int64_t value,int64_t& option_name){
    option_name= value;
    int64Value* new_val=new int64Value(&option_name);
    return new_val;
}
/**
 * @brief 返回uint64Value的字符串值
 * 
 * @return string 
 */
string uint64Value:: String(){
    std::stringstream ss;
    ss<<std::hex<<(*value);
    return ss.str();
    // return std::to_string(*value);
}
/**
 * @brief 设置uint64Value类型的值
 * 
 * @param value 
 */
void uint64Value:: Set(string value){
    int64_t v=std::stoull(value);
   *(this->value)=v;
}
/**
 * @brief 返回uint64Value的类型
 * 
 * @return string 
 */
string uint64Value:: Type(){
    return "uint64";
}
/**
 * @brief 新建uint64Value类型
 * 
 * @param value 
 * @param option_name 
 * @return uint64Value*
 */
uint64Value* newUint64Value(uint64_t value,uint64_t& option_name){
    option_name= value;
    uint64Value* new_val=new uint64Value(&option_name);
    return new_val;
}

/**
 * @brief 返回stringSliceValue的字符串值
 * 
 * @return string 
 */
string stringSliceValue::String(){
    std::ostringstream strout;
    if(!(*value).empty()){
        for(auto it=0;it<(*value).size()-1;it++){
            strout<<(*value)[it]<<" ";
        }
        strout<<(*value)[(*value).size()-1];
        return '['+strout.str()+']';
    }else{
        return "[]";
    }
    
}
/**
 * @brief 设置stringSliceValue类型的值
 * 
 * @param value 
 */
void stringSliceValue::Set(string value){
    if(!changed){
        vector<string> newvalue{value};
        *(this->value)=newvalue;
        changed=true;
    }else{
        this->value->emplace_back(value);
    }
}
/**
 * @brief 返回stringSliceValue的类型
 * 
 * @return string 
 */
string stringSliceValue::Type(){
    return "stringSlice";
}
/**
 * @brief 在stringSliceValue中添加值
 * 
 * @param val 
 */
void stringSliceValue::Append(string val){
    value->emplace_back(val);
}
/**
 * @brief 替换stringSliceValue中的字符串数组
 * 
 * @param newval 
 */
void stringSliceValue::Repalce(vector<string> newval){
    *value=newval;
}
/**
 * @brief 返回stringSliceValue的字符串数组
 * 
 * @return vector<string> 
 */
vector<string> stringSliceValue::GetSlice(){
    return *value;
}
/**
 * @brief 新建stringSliceValue类型
 * 
 * @param value stringSliceValue的值
 * @param option_name 值存储的位置
 * @return stringSliceValue* 
 */
stringSliceValue* newStringSliceValue(vector<string> value,vector<string>& option_name){
    option_name=value;
    stringSliceValue* newvar=new stringSliceValue(&option_name);
    return newvar;
}
