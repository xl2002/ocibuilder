#if !defined(UTILS_PARSE_LINE_PARSERS_H)
#define UTILS_PARSE_LINE_PARSERS_H
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <regex>
#include "utils/parse/dockerfileparse.h"
#include "utils/common/regexp.h"
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>> 
parseStringsWhitespaceDelimited(const std::string& rest, std::shared_ptr<Directive>d);
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>>
parseLabel(const std::string& rest, std::shared_ptr<Directive> d);
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>> 
parseMaybeJSONToList(const std::string& rest,const std::shared_ptr<Directive> d);
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>>
parseEnv(const std::string& rest, std::shared_ptr<Directive> d);
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>> 
parseMaybeJSON(const std::string& rest,const std::shared_ptr<Directive> d);
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>> 
parseIgnore(const std::string& rest,const std::shared_ptr<Directive> d);
#endif // PARSE_LINE_PARSERS_H
