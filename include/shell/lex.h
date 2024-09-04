#if !defined(SHELL_LEX_H)
#define SHELL_LEX_H
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <tuple>
#include <functional>
#include "cobra/error.h"

struct shellWord;
class Lex{
    public:
    // 公共成员变量，与 Go 结构体中的字段对应
    char escapeToken;
    bool RawQuotes=false;
    bool RawEscapes=false;
    bool SkipProcessQuotes=false;
    bool SkipUnsetEnv=false;

    std::tuple<std::string, std::vector<std::string>> process(std::string word, std::map<std::string, std::string> env);
    std::shared_ptr<shellWord> init(std::string word, std::map<std::string, std::string> env);
    std::vector<std::string>ProcessWords(std::string word,std::vector<std::string> env);


};

struct shellWord {
    std::stringstream ss;
    // 因为 C++ 没有直接对应的 scanner 类型，这里可能需要自定义一个 scanner 类或使用现有的解析器
    std::map<std::string, std::string> envs;   // 环境变量
    char escapeToken;                          // 转义字符
    bool rawQuotes=false;                            // 是否处理引号原始字符串
    bool rawEscapes=false;                           // 是否处理转义原始字符串
    bool skipUnsetEnv=false;                         // 是否跳过未设置的环境变量
    bool skipProcessQuotes=false;                    // 是否跳过处理引号
    std::map<std::string, std::string> matches;             // 匹配的字符串集合
    shellWord() = default;

    std::tuple<std::string,std::vector<std::string>>process(std::string source);
    std::tuple<std::string, std::vector<std::string>> processStopOn(char stopChar);
    std::string processDollar();
    std::string processName();
    std::string processSingleQuote();
    std::string processDoubleQuote();
};
std::shared_ptr<Lex> NewLex(const char& escapeToken);
std::map<std::string, std::string> BuildEnvs(const std::vector<std::string>& env);

class wordsStruct{

    public:
    std::string word;
    std::vector<std::string> words;
    bool inWord;
    wordsStruct() = default;
    void addChar(char ch);
    void addRawChar(char ch);
    void addString(const std::string& str);
    void addRawString(const std::string& str);
    std::vector<std::string> getWords();
};


#endif 
