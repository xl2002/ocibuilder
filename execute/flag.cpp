/**
 * @file flag.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief flag.h头文件对应的源文件，对声明的方法进行定义
 * @version 0.1
 * @date 2024-06-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "flag.h"


// Flagset::Flagset(){

// }
/**
 * @brief 根据字符串变量构建字符串标志
 * 
 * StringVar定义一个具有指定名称、默认值和用法字符串的字符串标志。
 */
void Flagset::StringVar(){

}

/**
 * @brief 根据字符数组构建字符串标志
 * 
 * StringArrayVar定义一个具有指定名称、默认值和用法字符串的字符串标志。
 */
void Flagset::StringArrayVar(){

}

/**
 * @brief 根据字符串构建字符串标志
 * 
 * string 定义具有指定名称、默认值和用法字符串的字符串标志。
 */
void Flagset::String(){

}

/**
 * @brief 根据bool标志进行构建
 * 
 * BoolVar 定义一个具有指定名称、默认值和用法字符串的布尔标志。
 */
void Flagset::BoolVar(){

}

/**
 * @brief 将构建好的标志集添加到命令结构中
 * 
 * AddFlagSet 将一个 FlagSet 添加到另一个 FlagSet。
 * 如果 f 中已存在标志，则 newSet 中的标志将被忽略。
 * @param flags 准备添加进入的标志集
 */
void Flagset::AddFlagSet(Flagset& flags){

}

/**
 * @brief 对简化的标志进行标准化
 * 
 * NormalizeFunc 允许您添加一个可以翻译标志名称的函数。
 * 添加到FlagSet的标志将被翻译，然后当任何东西试图查找也将被翻译的标志时。
 */
void Flagset::NormalizeFunc(){
    
} 
