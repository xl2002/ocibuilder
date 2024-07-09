#ifndef VALUE_H
#define VALUE_H
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
using std::string;
using std::vector;

class Value {
    public:
    ~Value()=default;
    virtual string String()=0;
    virtual void Set(string value)=0;
    virtual string Type()=0;
};

class StringArrayValue:public Value{
    public:
    vector<string>* value;
    bool changed=false;
    StringArrayValue(vector<string>* newvalue):value(newvalue),changed(false){};
    string String() override;
    void Set(string value) override;
    string Type() override;
    void Append(string val);
    void Repalce(vector<string> newval);
    vector<string> GetSlice();
};

class StringValue: public Value{
    public:
    string* value;
    StringValue(string* s):value(s){};
    string String() override;
    void Set(string value) override;
    string Type() override;
};

class BoolValue:public Value{
    public:
    bool* value=nullptr;
    BoolValue(bool* newval):value(newval){};
    string String() override;
    void Set(string value) override;
    string Type() override;
    bool IsBoolFlag();
};

class IntValue:public Value{
    public:
    int* value=nullptr;
    IntValue(int* new_val):value(new_val){};
    string String() override;
    void Set(string value) override;
    string Type() override;
};

class stringSliceValue:public Value{
    public:
    vector<string>* value;
    bool changed=false;
    stringSliceValue(vector<string>* new_val):value(new_val),changed(false){};
    string String() override;
    void Set(string value) override;
    string Type() override;
    void Append(string val);
    void Repalce(vector<string> newval);
    vector<string> GetSlice();
};

StringValue* newStringValue(string value, string &option_name);
StringArrayValue* newStringArrayValue(vector<string> value,vector<string>& option_name);
BoolValue* newBoolValue(bool value,bool& option_name);
IntValue* newIntValue(int value,int& option_name);
stringSliceValue* newStringSliceValue(vector<string> value,vector<string>& option_name);

#endif