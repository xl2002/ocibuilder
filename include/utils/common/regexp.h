#if !defined(UTILS_COMMON_REGEXP_H)
#define UTILS_COMMON_REGEXP_H
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
class Regexp{
    public:
    std::once_flag once;
    std::shared_ptr<std::regex> regexp=std::make_shared<std::regex>();
    std::string val;
    Regexp()=default;
    Regexp(const std::string& value) : val(value) {};
    void MustCompile();
    void compile();
    std::string Expand(const std::string& dst, const std::string& tmpl, const std::string& src, const std::vector<int>& match);
    std::string Find(const std::string& b);
    std::vector<int> FindIndex(const std::string& s);
    std::vector<std::string> FindAll(const std::string& b,int n);
    std::vector<std::pair<int, int>> FindAllIndex(const std::string& b, int n);
    std::vector<std::vector<std::string>> FindAllSubmatch(const std::string& s, int n);\
    std::vector<std::vector<int>> FindAllSubmatchIndex(const std::string& s, int n);
    std::vector<int> FindReaderIndex(std::istream& r);
    std::vector<int> FindReaderSubmatchIndex(std::istream& r);
    std::vector<std::string> FindSubmatch(const std::string& s);
    std::vector<int> FindSubmatchIndex(const std::string& s);
    std::tuple<std::string, bool> LiteralPrefix();
    bool Match(const std::string& str);
    bool MatchReader(std::istream& stream);
    int NumSubexp();
    std::string ReplaceAll(const std::string& src, const std::string& repl);
    std::string ReplaceAllFunc(const std::string& src, std::function<std::string(const std::string&)> repl);
    std::vector<uint8_t> ReplaceAllLiteral(const std::vector<uint8_t>& src, const std::vector<uint8_t>& repl);
    std::vector<std::string> Split(const std::string& s, int n);
    std::string String();
    std::shared_ptr<std::regex> GetRegex();
};
extern bool precompile;
std::shared_ptr<Regexp> Delayed(const std::string& val);
#endif // REGEXP_REGEXP_H
