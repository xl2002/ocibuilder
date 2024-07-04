#include "args.h"
/// @brief 
/// @param n 
/// @return 
function<bool(Command&,vector<string>&)> MaximumNArgs(int n){
    return [n](Command& cmd,vector<string>& args)->bool{
        if(args.size()>n){
            throw error_mismatch("accepts at most "+to_string(n)+ " arg(s), received "+to_string(args.size()));
            return false;
        }
        return true;
    };
}