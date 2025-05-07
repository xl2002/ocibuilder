#include "image/buildah/util.h"

/**
 * @brief 检查镜像引用是否符合特定条件
 * @param ref 要检查的镜像引用指针
 * @param sc 系统上下文指针
 * @param func 条件判断函数
 * @return bool 返回是否满足条件
 * @details 此函数作为条件检查的通用模板，实际逻辑由传入的func函数实现
 */
bool isReferenceSomething(
    std::shared_ptr<ImageReference_interface> ref,
    std::shared_ptr<SystemContext> sc,
    std::function<bool(std::shared_ptr<ImageReference_interface>,std::shared_ptr<SystemContext>)> func
){
    return false;
}
/**
 * @brief 检查镜像引用是否被阻止
 * @param ref 要检查的镜像引用指针
 * @param sc 系统上下文指针
 * @return bool 返回镜像引用是否被阻止
 * @details 主要检查docker类型的镜像引用是否被阻止，通过递归调用isReferenceSomething实现
 */
bool isReferenceBlocked(std::shared_ptr<ImageReference_interface> ref,std::shared_ptr<SystemContext> sc){
    if(ref!=nullptr && ref->Transport()!=nullptr){
        auto name=ref->Transport()->Name();
        if(name=="docker"){
            return isReferenceSomething(ref,sc,isReferenceBlocked);
        }

    }
    return false;
}

/**
 * @brief 深拷贝字符串映射
 * @param src 源字符串映射
 * @return std::map<std::string,std::string> 返回新的字符串映射副本
 * @details 此函数创建并返回输入映射的完全独立副本，确保修改副本不会影响原始映射
 */
std::map<std::string,std::string> copyStringStringMap(std::map<std::string,std::string> src){
    std::map<std::string,std::string> dst;
    for(auto& e:src){
        dst[e.first]=e.second;
    }
    return dst;
}
