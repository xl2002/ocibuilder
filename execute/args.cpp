#include "args.h"
/**
 * @brief 设置参数列表最大的数量
 * 
 * @param n 参数列表的最大长度
 * @return function<bool(Command&,vector<string>&)> 返回可调用对象function
 */
function<bool(Command&,vector<string>&)> MaximumNArgs(int n){
    return [n](Command& cmd,vector<string>& args)->bool{
        if(args.size()>n){
            throw error_mismatch("accepts at most "+to_string(n)+ " arg(s), received "+to_string(args.size()));
            return false;
        }
        return true;
    };
}