/**
 * @file error.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 用于声明自定义的异常处理类和方法
 * @version 0.1
 * @date 2024-07-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ERROR_H
#define ERROR_H
#include <stdexcept>
#include <string>
/**
 * @brief 未匹配的异常
 * 
 */
class error_mismatch:public std::runtime_error{
    public:
    error_mismatch(const std::string &s):std::runtime_error(s){};
};


#endif