#include "value.h"

string StringValue::String(){
    return *value;
}
void StringValue::Set(string value){
    *(this->value)=value;
}
string StringValue::Type(){
    return "string";
}
StringValue* newStringValue(string value, string &option_name){
    option_name=value;
    StringValue* var=new StringValue(&option_name);
    return var;
}

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

void StringArrayValue::Set(string value){
    if(!changed){
        vector<string>newvalue{value};
        *(this->value)=newvalue;
        changed=true;
    }else{
        this->value->emplace_back(value);
    }
}

string StringArrayValue::Type(){
    return "stringArray";
}

void StringArrayValue::Append(string val){
    value->emplace_back(val);
}

void StringArrayValue::Repalce(vector<string> newval){
    *value=newval;
}

vector<string> StringArrayValue::GetSlice(){
    return *value;
}

StringArrayValue* newStringArrayValue(vector<string> value,vector<string>& option_name){
    option_name=value;
    StringArrayValue* newvar=new StringArrayValue(&option_name);
    return newvar;
}

string BoolValue::String(){
    if(*value){
        return "true";
    }else{
        return "false";
    }
}

void BoolValue::Set(string value){
    if(value=="true"){
        *(this->value)=true;
    }
    if(value=="false"){
        *(this->value)=false;
    }
}

string BoolValue::Type(){
    return "bool";
}
bool BoolValue::IsBoolFlag(){
    return true;
}

BoolValue* newBoolValue(bool value,bool& option_name){
    option_name=value;
    BoolValue* new_val=new BoolValue(&option_name);
    return new_val;
}

string IntValue::String(){
    return std::to_string(*value);
}
void IntValue::Set(string value){
    *(this->value)=std::stoi(value);
}
string IntValue::Type(){
    return "int";
}

IntValue* newIntValue(int value,int& option_name){
    option_name= value;
    IntValue* new_val=new IntValue(&option_name);
    return new_val;
}

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
void stringSliceValue::Set(string value){
    if(!changed){
        vector<string> newvalue{value};
        *(this->value)=newvalue;
        changed=true;
    }else{
        this->value->emplace_back(value);
    }
}
string stringSliceValue::Type(){
    return "stringSlice";
}

void stringSliceValue::Append(string val){
    value->emplace_back(val);
}

void stringSliceValue::Repalce(vector<string> newval){
    *value=newval;
}

vector<string> stringSliceValue::GetSlice(){
    return *value;
}

stringSliceValue* newStringSliceValue(vector<string> value,vector<string>& option_name){
    option_name=value;
    stringSliceValue* newvar=new stringSliceValue(&option_name);
    return newvar;
}