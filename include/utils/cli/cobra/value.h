/**
 * @file value.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief value.h 声明与标签值相关的结构和方法
 * @version 0.1
 * @date 2024-07-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef UTILS_CLI_COBRA_VALUE_H
#define UTILS_CLI_COBRA_VALUE_H
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <cstdint>
#include <algorithm>
using std::string;
using std::vector;
/**
 * @brief 抽象基类
 * <p>为了能够适用不同标签对值类型不同的需求
 */
class Value_interface {
    public:
    virtual ~Value_interface()=default;
    virtual string String()=0;
    virtual void Set(string value)=0;
    virtual string Type()=0;
};
/**
 * @brief 保存字符串数组的抽象基类的子类
 * 
 */
class StringArrayValue:public Value_interface{
    public:
    vector<string>* value=nullptr;
    bool changed=false;
    StringArrayValue(vector<string>* newvalue):value(newvalue),changed(false){};
    string String() override;
    void Set(string value) override;
    string Type() override;
    void Append(string val);
    void Repalce(vector<string> newval);
    vector<string> GetSlice();
};
/**
 * @brief 保存字符串的抽象基类的子类
 * 
 */
class StringValue: public Value_interface{
    public:
    string* value=nullptr;
    StringValue(string* s):value(s){};
    string String() override;
    void Set(string value) override;
    string Type() override;
};
/**
 * @brief 保存布尔类型的抽象基类的子类
 * 
 */
class BoolValue:public Value_interface{
    public:
    bool* value=nullptr;
    BoolValue(bool* newval):value(newval){};
    string String() override;
    void Set(string value) override;
    string Type() override;
    bool IsBoolFlag();
};
/**
 * @brief 保存int类型的抽象基类的子类
 * 
 */
class IntValue:public Value_interface{
    public:
    int* value=nullptr;
    IntValue(int* new_val):value(new_val){};
    string String() override;
    void Set(string value) override;
    string Type() override;
};
/**
 * @brief 保存int64类型的抽象基类的子类
 * 
 */
class int64Value:public Value_interface{
    public:
    int64_t* value=nullptr;
    int64Value(int64_t* new_val):value(new_val){};
    string String() override;
    void Set(string value) override;
    string Type() override;
};
/**
 * @brief 保存uint64类型的抽象基类的子类
 * 
 */
class uint64Value:public Value_interface{
    public:
    uint64_t* value=nullptr;
    uint64Value(uint64_t* new_val):value(new_val){};
    string String() override;
    void Set(string value) override;
    string Type() override;
};
/**
 * @brief 保存字符串和bool类型的抽象基类的子类
 * 
 */
class stringSliceValue:public Value_interface{
    public:
    vector<string>* value=nullptr;
    bool changed=false;
    stringSliceValue(vector<string>* new_val):value(new_val),changed(false){};
    string String() override;
    void Set(string value) override;
    string Type() override;
    void Append(string val);
    void Repalce(vector<string> newval);
    vector<string> GetSlice();
};

std::shared_ptr<StringValue> newStringValue(string value, string &option_name);
std::shared_ptr<StringArrayValue> newStringArrayValue(vector<string> value,vector<string>& option_name);
std::shared_ptr<BoolValue> newBoolValue(bool value,bool& option_name);
std::shared_ptr<IntValue> newIntValue(int value,int& option_name);
std::shared_ptr<int64Value> newInt64Value(int64_t value,int64_t& option_name);
std::shared_ptr<uint64Value> newUint64Value(uint64_t value,uint64_t& option_name);
std::shared_ptr<stringSliceValue> newStringSliceValue(vector<string> value,vector<string>& option_name);

#endif