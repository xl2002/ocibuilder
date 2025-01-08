#include "image/types/define/pull.h"


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
bool PullPolicy::Validate() {
    auto p=this->value;
    if(p==PullIfMissing||p==PullAlways||p==PullIfNewer||p==PullNever) {
        return true;
    } else {
        return false;
    }
}
// map<string,Pull_Policy>PolicyMap ={
// 	{"missing",PullIfMissing},
// 	{"always", PullAlways},
// 	{"never",  PullNever},
// 	{"ifnewer", PullIfNewer}
// };

// 解析拉取策略函数
std::shared_ptr<PullPolicy> ParsePullPolicy(const std::string& s) {
    if (s == "always" || s == "Always") {
        return std::make_shared<PullPolicy>(Pull_Policy::PullAlways);
    } else if (s == "missing" || s == "Missing" || s == "ifnotpresent" || s == "IfNotPresent" || s.empty()) {
        return std::make_shared<PullPolicy>(Pull_Policy::PullIfMissing);
    } else if (s == "newer" || s == "Newer" || s == "ifnewer" || s == "IfNewer") {
        return std::make_shared<PullPolicy>(Pull_Policy::PullIfNewer);
    } else if (s == "never" || s == "Never") {
        return std::make_shared<PullPolicy>(Pull_Policy::PullNever);
    } else {
        throw std::runtime_error("unsupported pull policy: " + s); // 抛出异常
    }
}