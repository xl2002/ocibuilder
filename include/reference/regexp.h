#if !defined(REFERENCE_REGEXP_H)
#define REFERENCE_REGEXP_H
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <regex>
#include <mutex>
#include <iostream>
#include <locale>
#include <codecvt>
#include <functional>
#include "regexp/regexp.h"

extern std::shared_ptr<Regexp> anchoredIdentifierRegexp;
extern std::shared_ptr<Regexp> anchoredTagRegexp;
void init_regexp();
std::string expression(std::initializer_list<std::string> res);
std::string anchored(std::initializer_list<std::string> res);
std::string capture(std::initializer_list<std::string> res);
std::string group(std::initializer_list<std::string> res);
std::string repeated(std::initializer_list<std::string> res);
std::string optional(std::initializer_list<std::string> res);
std::string expression(std::initializer_list<std::string> res);
std::string literal(const std::string& s);
#endif // REFERENCE_REGEXP_H
