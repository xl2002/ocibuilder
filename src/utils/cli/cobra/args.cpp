#include "utils/cli/cobra/args.h"
#include "utils/logger/ProcessSafeLogger.h"
/**
 * @brief 设置参数列表最大的数量
 * 
 * @param n 参数列表的最大长度
 * @return function<bool(Command&,vector<string>&)> 返回可调用对象function
 */
function<bool(Command&,vector<string>&)> MaximumNArgs(int n){
    return [n](Command& cmd,vector<string>& args)->bool{
        if(args.size()>n){
            throw myerror("accepts at most "+to_string(n)+ " arg(s), received "+to_string(args.size()));
            return false;
        }
        return true;
    };
    // error_code
}
/**
 * @brief 设置参数列表最小的数量
 * 
 * @param n 
 * @return function<bool(Command&,vector<string>&)> 
 */
function<bool(Command&,vector<string>&)> MinimumNArgs(int n){
    return [n](Command& cmd,vector<string>& args)->bool{
        if(args.size()<n){
            throw myerror("accepts at least "+to_string(n)+ " arg(s), only received "+to_string(args.size()));
            return false;
        }
        return true;
    };
    // error_code
}