#if !defined(PAARSE_DOCKERFILEPARSE_H)
#define PAARSE_DOCKERFILEPARSE_H
#include <string> 
#include <vector>
#include <memory>
#include <map>
#include <sstream>
#include <regex>
#include "regexp/regexp.h"
#include "parse/buildkitparser.h"
class Node{
    public:
    std::string Value;
    std::shared_ptr<Node> Next=nullptr;
    std::vector<std::shared_ptr<Node>> Children;
    std::vector<Heredoc> Heredocs;
    std::map<std::string,bool> Attributes;
    std::string Original;
    std::vector<std::string> Flags;
    int StartLine=0;
    int EndLine=0;
    Node()=default;
    std::string Dump() const;
    void lines(const int start, const int end);
    bool canContainHeredoc() const;
    void AddChild(std::shared_ptr<Node> child, int startLine, int endLine);
};

// using rune=int32_t;
class Result{
    public:
    std::shared_ptr<Node> AST=nullptr;
    char EscapeToken=0;
    std::string Platform;
    std::vector<std::string> Warnings;
};

class Directive{
    public:
    char escapeToken=0;//记录跳过的token
    std::string platformToken;
    std::shared_ptr<std::regex> lineContinuationRegex=nullptr; // 当前行延续正则表达式
    bool processingComplete=false;         // 是否完成指令处理
    bool escapeSeen=false;                 // 是否看到转义指令
    bool platformSeen=false;               // 是否看到平台指令

    void setEscapeToken(const std::string& s);
    void setPlatformToken(const std::string& s);
    void possibleParserDirective(const std::string& line);
};


extern std::shared_ptr<Regexp> tokenEscapeCommand;
extern std::shared_ptr<Regexp> tokenPlatformCommand;
extern std::shared_ptr<Regexp> tokenWhitespace;
extern std::shared_ptr<Regexp> tokenComment;
extern std::map<std::string, bool> heredocDirectives;
extern std::string defaultPlatformToken;
extern const char DefaultEscapeToken;
extern std::map<std::string, bool> heredocCompoundDirectives;
namespace dockerfilecommand {
    extern const std::string Add;
    extern const std::string Arg;
    extern const std::string Cmd;
    extern const std::string Copy;
    extern const std::string Entrypoint;
    extern const std::string Env;
    extern const std::string Expose;
    extern const std::string From;
    extern const std::string Healthcheck;
    extern const std::string Label;
    extern const std::string Maintainer;
    extern const std::string Onbuild;
    extern const std::string Run;
    extern const std::string Shell;
    extern const std::string StopSignal;
    extern const std::string User;
    extern const std::string Volume;
    extern const std::string Workdir;
};






std::shared_ptr<Result> Parse(std::stringstream& rwc);
std::shared_ptr<Node> newNodeFromLine(const std::string& line, std::shared_ptr<Directive> directive);
std::string trimComments(const std::string& src);
std::string trimWhitespace(const std::string& src);
std::string trimLeadingWhitespace(const std::string& src);
std::string trimLeadingTabs(const std::string& src);
std::string trimNewline(const std::string& src);
std::tuple<std::string,bool> trimContinuationCharacter(std::string line, std::shared_ptr<Directive> d);
bool isEmptyContinuationLine(const std::string& line);
std::vector<Heredoc>heredocsFromLine(const std::string& line);
std::string processLine(std::shared_ptr<Directive> d, const std::string& token, bool stripLeftWhitespace);

#endif // PAARSE_DOCKERFILEPARSE_H
