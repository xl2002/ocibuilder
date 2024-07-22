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
#ifndef COBRA_ERROR_H
#define COBRA_ERROR_H
#include <stdexcept>
#include <string>
#include <iostream>
using std::cerr;
using std::endl;
/**
 * @brief 未匹配的异常
 * 
 */
class myerror:public std::exception {
    public:
    myerror() = default;
    myerror(const std::string &message):message_(message) {};

    virtual const char* what() const noexcept override {
        return message_.c_str();
    }
    void logerror() const{
        cerr<<"Error: "<<what()<<endl;
    }
    void AddError(const std::string& err) {
        message_ += err;
    }
    private:
    std::string message_;
};

// class error{
//     private:
//     std::string error_str;

//     public:

// };

#endif