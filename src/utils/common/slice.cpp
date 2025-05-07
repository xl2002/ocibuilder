/**
 * @file slice.cpp
 * @brief Slice操作实现文件
 * @details 提供Slice类的创建和操作方法
 */

#include "utils/common/slice.h"

/**
 * @brief 创建新的Slice对象
 * @param values 初始字符串向量
 * @return 返回包含给定值的共享指针Slice对象
 */
std::shared_ptr<Slice> NewSlice(std::vector<std::string> values) {
    auto re = std::make_shared<Slice>();
    re->Values=values;
    return re;
}
/**
 * @brief 设置Slice的值
 * @param values 要设置的新字符串向量
 */
void Slice::Set(std::vector<std::string> values){
    Values=values;
}

/**
 * @brief 获取Slice的值
 * @return 返回当前字符串向量，如果为空则返回空向量
 */
std::vector<std::string> Slice::Get(){
    if(Values.size()==0){
        return std::vector<std::string>();
    }
    return Values;
}
