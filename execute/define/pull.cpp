#include "define/pull.h"


/**
 * @brief 将 Pull_Policy 类型转换为字符串类型
 * 
 * @param value Pull_Policy 类型
 * @return string 转换后的字符串
 * @throws std::cerr 如果 value 不是 Pull_Policy 类型中的有效值，则抛出错误信息
 */
string PullPolicy::String(){
    switch(value) {
            case PullIfMissing:
                return "missing";
            case PullAlways:
                return "always";
            case PullIfNewer:
                return "ifnewer";
            case PullNever:
                return "never";
            default:
                std::cerr << "unrecognized policy " << value << std::endl;
                return "";
        }
}

map<string,Pull_Policy>PolicyMap ={
	{"missing",PullIfMissing},
	{"always", PullAlways},
	{"never",  PullNever},
	{"ifnewer", PullIfNewer}
};