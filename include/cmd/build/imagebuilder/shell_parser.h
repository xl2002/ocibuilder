#if !defined(CMD_BUILD_IMAGEBUILDER_SHELL_PARSER_H)
#define CMD_BUILD_IMAGEBUILDER_SHELL_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <functional>
#include <tuple>
#include <boost/spirit/include/classic_core.hpp> // 用于模拟Go中的scanner.Scanner
#include <sstream>

// 定义shellWord结构体，包含word字符串，scanner，环境变量envs，和pos位置
class shellWord {
    public:
    std::string word;                            // 字符串
    std::stringstream scanner;   // 模拟Go语言中的scanner.Scanner
    std::vector<std::string> envs;               // 环境变量列表
    int pos=0;                                     // 位置索引
    std::tuple<std::string, std::vector<std::string>> process();
    std::tuple<std::string, std::vector<std::string>> processStopOn(int stopChar);
    std::string processSingleQuote();
    std::string processDoubleQuote();
    std::string processDollar();
    std::string processName();
    std::string getEnv(const std::string& name);

};

class wordsStruct{
    public:
    std::string word;
    std::vector<std::string> words;
    bool inWord=false;
    std::vector<std::string> getWords();
    void addChar(char ch);
    void addRawChar(char ch);
    void addString(const std::string& str);
    void addRawString(const std::string& str);
};
std::string ProcessWord(std::string word,std::vector<std::string>env);
std::vector<std::string> ProcessWords(std::string word,std::vector<std::string>env);
#endif // IMAGEBUILDER_SHELL_PARSER_H)
