
#ifndef ERROR_H
#define ERROR_H
#include <stdexcept>
#include <string>
class error_mismatch:public std::runtime_error{
    public:
    error_mismatch(const std::string &s):std::runtime_error(s){};
};


#endif