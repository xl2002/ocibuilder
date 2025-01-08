#include "utils/parse/dockerfileparse.h"
#include "utils/parse/line_parsers.h"
#include "utils/common/error.h"
#include "utils/common/go/string.h"
#include "filesys/system/lcow_unix.h"
#include <algorithm>
#include <iomanip>
#include <tuple>
#include "utils/parse/split_command.h"
#include "utils/cli/cobra/lex.h"
#include "utils/parse/buildkitparser.h"
std::string Node::Dump() const {
    std::ostringstream oss;
    oss << Value;

    if (!Flags.empty()) {
        oss << " ";
        oss <<'"' <<Join(Flags, ",")<<'"';
    }

    for (const auto& child : Children) {
        oss << "(" << child->Dump() << ")\n";
    }

    // Assuming Heredocs is a member of Node
    for (const auto& doc : Heredocs) {
        oss << "(" 
            << doc.Name << "-" 
            << doc.Content << "-" 
            << doc.FileDescriptor << "-" 
            << std::boolalpha << doc.Expand << "-" 
            << doc.Chomp 
            << ")\n";
    }

    // Handle the linked list of Nodes
    auto current = Next;
    while (current) {
        if (!current->Children.empty()) {
            oss << " " << current->Dump();
        } else {
            oss << " " <<'"'<<current->Value<<'"';
        }
        current = current->Next;
    }

    return TrimSpace(oss.str());
}

void Node::lines(const int start, const int end) {
    StartLine=start;
    EndLine=end;
}
bool Node::canContainHeredoc() const {
    // Convert Value to lowercase
    std::string lowerValue = toLower(Value);
    // std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);

    // Check for compound commands like ONBUILD
    auto it = heredocCompoundDirectives.find(lowerValue);
    if (it != heredocCompoundDirectives.end()) {
        if (Next != nullptr && !Next->Children.empty()) {
            auto nextNode = Next->Children[0];
            // return checkHeredocAllowed(nextNode);
        }
    }

    // Check if current node allows heredocs
    if (heredocDirectives.find(lowerValue) == heredocDirectives.end()) {
        return false;
    }

    // Check if JSON attribute is present
    auto attrIt = Attributes.find("json");
    if (attrIt != Attributes.end() && attrIt->second) {
        return false;
    }

    return true;
}
// Adds a new child node and updates line information
void Node::AddChild(std::shared_ptr<Node> child, int startLine, int endLine) {
    // Update the child's line information
    child->lines(startLine, endLine);

    // Update the current node's line information
    if (StartLine < 0) {
        StartLine = startLine;
    }
    EndLine = endLine;

    // Add the child to the list of children
    Children.push_back(child);
}

// Function to handle possible parser directives
// 定义用于存储指令的映射
// using DispatchFunction=std::function<std::tuple<std::shared_ptr<Node>,std::map<std::string,bool>>(std::string,std::shared_ptr<Directive>)>;
// std::map<std::string, DispatchFunction> dispatch={
//     {dockerfilecommand::From,parseStringsWhitespaceDelimited},
//     {dockerfilecommand::Label,parseLabel},
//     {dockerfilecommand::Copy,parseMaybeJSONToList},
//     {dockerfilecommand::Env,parseEnv},
//     {dockerfilecommand::Expose,parseStringsWhitespaceDelimited},
//     {dockerfilecommand::Entrypoint,parseMaybeJSON},
//     {dockerfilecommand::Volume,parseMaybeJSONToList}
// };

// std::shared_ptr<Regexp> tokenEscapeCommand=Delayed(R"(^#[ \t]*escape[ \t]*=[ \t]*(?P<escapechar>.).*$)");
// std::shared_ptr<Regexp> tokenPlatformCommand=Delayed(R"(^#[ \t]*platform[ \t]*=[ \t]*(?P<platform>.*)$)");
// std::shared_ptr<Regexp> tokenWhitespace=Delayed(R"([\t\v\f\r ]+)");
// std::shared_ptr<Regexp> tokenComment=Delayed(R"(^#.*$)");
// 默认转义符号
// const char DefaultEscapeToken = '\\';

// 默认平台标识符，假设为构建的默认平台
// std::string defaultPlatformToken = "windows"; // 用实际的默认平台替代

// 定义包含 heredoc 的指令集合
// std::map<std::string, bool> heredocDirectives = {
//     {"ADD", true},
//     {"COPY", true},
//     {"RUN", true}
// };

// 定义允许包含指令的指令集合
// std::map<std::string, bool> heredocCompoundDirectives = {
//     {"ONBUILD", true}
// };
void Directive::setEscapeToken(const std::string& s){
    if (s != "`" && s != "\\") {
            throw myerror("invalid ESCAPE '" + s + "'. Must be ` or \\");
        }
    escapeToken = s[0];
    lineContinuationRegex = std::make_shared<std::regex>("\\\\" + s + "[ \t]*$");
}
// Member function to set platform token
void Directive::setPlatformToken(const std::string& s) {
    std::string lower_s = toLower(s);
    std::vector<std::string> valid = { getRuntimeGOOS() };  // Assuming getRuntimeGOOS() returns the current OS.

    if (isLCOWSupported()) {
        valid.push_back("linux");
    }

    if (std::find(valid.begin(), valid.end(), lower_s) != valid.end()) {
        platformToken = lower_s;
    } else {
        throw myerror("invalid PLATFORM '" + s + "'. Must be one of " + vectorToString(valid));
    }
}

void Directive::possibleParserDirective(const std::string& line) {
    if (processingComplete) {
        return;
    }

    std::string lower_line = toLower(line);
    std::smatch match;

    // Check for escape token
    auto tecMatch=tokenEscapeCommand->FindSubmatch(lower_line);
    if (!tecMatch.empty()) {
        if (escapeSeen) {
            throw std::runtime_error("only one escape parser directive can be used");
        }
        escapeSeen = true;
        setEscapeToken(match.str());
        return;
    }

    // Check for platform token if LCOW is supported
    if (isLCOWSupported()) {
        auto tpcMatch = tokenPlatformCommand->FindSubmatch(lower_line);
        if (!tpcMatch.empty()) {
            if (platformSeen) {
                throw std::runtime_error("only one platform parser directive can be used");
            }
            platformSeen = true;
            setPlatformToken(match.str());
            return;
        }
    }

    processingComplete = true;
}

std::shared_ptr<Directive> NewDefaultDirective() {
    auto directive=std::make_shared<Directive>();
    directive->setEscapeToken(std::string(1, DefaultEscapeToken));
    directive->setPlatformToken(defaultPlatformToken);
    return directive;
}
std::shared_ptr<Result> Parse(std::stringstream& rwc) {
    std::string str=rwc.str();
    std::vector<uint8_t> origin(str.begin(),str.end());
    auto d=NewDefaultDirective();
    auto currentLine=0;
    auto root=std::make_shared<Node>();
    root->StartLine=-1;
    std::string line;
    std::vector<std::string> warnings;
    while (std::getline(rwc, line)) {
        ++currentLine;
        if (!line.empty() && line.back() == '\r') {
            line.pop_back(); // 去掉最后的 \r
        }
        // Handle large lines, not directly applicable as in Go, but this is a placeholder
        std::string bytesRead = line;

        if (currentLine == 1) {
            // Strip the byte-order-marker if present (not directly applicable in C++)
        }
        bytesRead=processLine(d, bytesRead, true);
        // Process the line
        auto startLine=currentLine;
        bool isEndOfLine;
        std::tie(bytesRead,isEndOfLine) = trimContinuationCharacter(bytesRead, d);
        if (bytesRead.empty()&& isEndOfLine) {
            continue;
        }

        // bool isEndOfLine = true; // Placeholder, should determine based on logic

        std::string accumulatedLine = bytesRead;
        bool hasEmptyContinuationLine = false;

        while (!isEndOfLine && std::getline(rwc, line)) {//用来识别一个多行命令
            // ++currentLine;

            // bytesRead = line;

            // if (isEmptyContinuationLine(bytesRead)) {
            //     hasEmptyContinuationLine = true;
            //     continue;
            // }

            // accumulatedLine += bytesRead;
            // accumulatedLine = trimContinuationCharacter(accumulatedLine, d);
        }

        if (hasEmptyContinuationLine) {
            warnings.push_back("[WARNING]: Empty continuation line found in:\n    " + accumulatedLine);
        }

        auto child = newNodeFromLine(bytesRead, d);
        if (child->canContainHeredoc()) {
            auto heredocs = heredocsFromLine(bytesRead);
            for (auto& heredoc : heredocs) {
                std::string terminator = heredoc.Name;
                bool terminated = false;
                while (std::getline(rwc, line)) {
                    auto stringread=line;
                    ++currentLine;
                    std::string possibleTerminator = trimNewline(stringread);
                    if(heredoc.Chomp){
                        possibleTerminator=trimLeadingTabs(possibleTerminator);
                    }
                    if (possibleTerminator == terminator) {
                        terminated = true;
                        break;
                    }
                    heredoc.Content += "\n" + stringread;
                }
                if (!terminated) {
                    throw myerror(heredoc.Name + ": unterminated heredoc");
                }
                child->Heredocs.emplace_back(heredoc); // Assuming heredocs are stored in Flags for simplicity
            }
        }

        root->AddChild(child, startLine, currentLine);
    }
    // auto children=root->Children;
    // Handle scanner errors, not directly applicable in C++
    if (rwc.bad()) {
        throw myerror("Stream error occurred");
    }

    if (!warnings.empty()) {
        warnings.push_back("[WARNING]: Empty continuation lines will become errors in a future release.");
    }
    auto ret=std::make_shared<Result>();
    ret->AST=root;
    ret->Warnings=warnings;
    ret->EscapeToken=d->escapeToken;
    ret->Platform=d->platformToken;
    return ret;
    // return {root, warnings};
}
std::vector<Heredoc>heredocsFromLine(const std::string& line) {
    auto shlex = NewLex('\\');
    shlex->RawQuotes = true;
    shlex->RawEscapes = true;
    shlex->SkipUnsetEnv = true;
    
    std::vector<std::string> words = shlex->ProcessWords(line,std::vector<std::string>{});

    std::vector<Heredoc> docs;
    for (const auto& word : words) {
        // Heredoc heredoc;
        // bool success;
        try
        {
            auto heredoc = ParseHeredoc(word);
            if(heredoc) docs.push_back(*heredoc);
        }
        catch(const myerror& e)
        {
            throw;
        }
    }

    return docs;
}
std::shared_ptr<Node> newNodeFromLine(const std::string& line, std::shared_ptr<Directive> directive) {
    // 将命令拆分为命令、标志和参数
    std::string cmd, args;
    std::vector<std::string> flags;
    std::tie(cmd, flags, args) = splitCommand(line);

    // 查找命令对应的处理函数
    auto it = dispatch.find(cmd);
    auto fn=it->second;
    if (it == dispatch.end()) {
        fn=parseIgnore; // 如果未找到命令，则使用默认的忽略函数
    }

    // CommandFunction fn = it->second;

    // 执行处理函数
    std::shared_ptr<Node> next;
    std::map<std::string, bool> attrs;
    std::tie(next, attrs) = fn(args, directive);

    // 返回构造的 Node
    // return std::make_shared<Node>(cmd, line, flags, next, attrs);
    auto newnode=std::make_shared<Node>();
    newnode->Value=cmd;
    newnode->Original=line;
    newnode->Flags=flags;
    newnode->Next=next;
    newnode->Attributes=attrs;
    return newnode;
}

std::string trimComments(const std::string& src) {
    static const std::regex tokenComment(R"(#.*?\r?$)", std::regex_constants::ECMAScript);
    std::string result = std::regex_replace(src, tokenComment, "");
    return result;
}
std::string trimWhitespace(const std::string& src) {
    std::string result = src;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    return result;
}
std::string trimLeadingWhitespace(const std::string& src) {
    std::string result = src;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    return result;
}
std::string trimLeadingTabs(const std::string& src) {
    std::string result = src;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
        return ch != '\t';
    }));
    return result;
}
std::string trimNewline(const std::string& src) {
    std::string result = src;
    result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
        return ch != '\r' && ch != '\n';
    }).base(), result.end());
    return result;
}
std::tuple<std::string,bool> trimContinuationCharacter(std::string line, std::shared_ptr<Directive> d) {
    std::smatch match;
    if (std::regex_search(line, match, *d->lineContinuationRegex)) {
        // 去除续行字符
        std::string trimmedLine = std::regex_replace(line, *d->lineContinuationRegex, "");
        return std::make_tuple(trimmedLine, false);
    }
    return std::make_tuple(line, true);
}
bool isEmptyContinuationLine(const std::string& line) {
    std::string trimmed = trimWhitespace(line);
    trimmed = trimComments(trimmed);
    return trimmed.empty();
}

std::string processLine(std::shared_ptr<Directive> d, const std::string& token, bool stripLeftWhitespace) {
    std::string processedToken = token;

    if (stripLeftWhitespace) {
        processedToken = trimWhitespace(processedToken);
    }

    auto ret_processedToken = trimComments(processedToken);
    // std::string directiveResult = d->possibleParserDirective(processedToken);
    try
    {
        d->possibleParserDirective(processedToken);
    }
    catch(const myerror& e)
    {
        throw;
    }
    return ret_processedToken;
}