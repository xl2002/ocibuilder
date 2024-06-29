/**
 * @file command.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对command.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "command.h"
/**
 * @brief Command类的列表构造函数
 * 对命令的名称，介绍，使用例子进行初始化的构造函数
 * @param name 命令的名称
 * @param Short 命令的简短介绍
 * @param Long 命令详细的介绍
 * @param example 命令的使用样例
 */
Command::Command(string& name,string& Short,string& Long, string& example):
                name(name),Short(Short),Long(Long),example(example) {}


/**
 * @brief 检查命令的参数
 * 是否为预期参数
 * @return true 参数列表正确
 * @return false 参数列表错误
 */
bool Command::Args(){
    return true;
}

/**
 * @brief 返回命令的标志集
 * 
 * @return Flagset& 返回命令中标志集的引用
 */
Flagset& Command::Flags(){
    return flags;
}

/**
 * @brief 返回命令的持久化标志集
 * 
 * @return Flagset& 返回命令中持久化标志集的引用
 */
Flagset& Command::PersistentFlags(){
    return persistent_flags;
}

/**
 * @brief 向命令中添加子命令
 * 
 * @return Command& 返回命令的本身引用
 */
Command& Command::AddCommand(initializer_list<Command>cmdlist){
    
    return *this;
}