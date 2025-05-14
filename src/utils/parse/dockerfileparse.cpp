#include "utils/parse/dockerfileparse.h"
#include "utils/parse/line_parsers.h"
#include "utils/common/error.h"
#include "utils/common/go/string.h"
#include "filesys/system/lcow_unix.h"
#include "utils/logger/ProcessSafeLogger.h"
#include <algorithm>
#include <iomanip>
#include <tuple>
#include "utils/parse/split_command.h"
#include "utils/cli/cobra/lex.h"
#include "utils/parse/buildkitparser.h"
/**
 * @brief 将节点内容转储为字符串表示
 * @details 递归转储节点及其子节点的内容，包括值、标志和heredoc信息
 * @return std::string 节点的字符串表示
 */
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

/**
 * @brief 设置节点的起始和结束行号
 * @param start 起始行号
 * @param end 结束行号
 */
void Node::lines(const int start, const int end) {
    StartLine=start;
    EndLine=end;
}
/**
 * @brief 检查节点是否可以包含heredoc
 * @details 检查节点类型、JSON属性和复合指令情况
 * @return bool 如果节点可以包含heredoc返回true，否则返回false
 */
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
/**
 * @brief 添加子节点并更新行号信息
 * @param child 要添加的子节点
 * @param startLine 子节点起始行号
 * @param endLine 子节点结束行号
 */
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
/**
 * @brief 设置转义字符
 * @details 设置Dockerfile解析使用的转义字符，只能是`或\
 * @param s 要设置的转义字符
 * @throws myerror 如果转义字符无效
 */
void Directive::setEscapeToken(const std::string& s){
    if (s != "`" && s != "\\") {
            logger->log_error("Invalid escape token: " + s);
            throw myerror("invalid ESCAPE '" + s + "'. Must be ` or \\");
        }
    escapeToken = s[0];
    lineContinuationRegex = std::make_shared<std::regex>("\\\\" + s + "[ \t]*$");
}
// Member function to set platform token
/**
 * @brief 设置平台标识符
 * @details 设置Dockerfile构建的目标平台
 * @param s 平台标识符字符串
 * @throws myerror 如果平台标识符无效
 */
void Directive::setPlatformToken(const std::string& s) {
    std::string lower_s = toLower(s);
    std::vector<std::string> valid = { getRuntimeGOOS() };  // Assuming getRuntimeGOOS() returns the current OS.

    if (isLCOWSupported()) {
        valid.push_back("linux");
    }

    if (std::find(valid.begin(), valid.end(), lower_s) != valid.end()) {
        platformToken = lower_s;
    } else {
        logger->log_error("Invalid platform token: " + s + ", valid platforms: " + vectorToString(valid));
        throw myerror("invalid PLATFORM '" + s + "'. Must be one of " + vectorToString(valid));
    }
}

/**
 * @brief 处理可能的解析器指令
 * @details 检查并处理Dockerfile中的解析器指令(escape/platform)
 * @param line 要检查的行内容
 */
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

/**
 * @brief 创建默认指令对象
 * @return std::shared_ptr<Directive> 包含默认escape和platform设置的指令对象
 */
std::shared_ptr<Directive> NewDefaultDirective() {
    auto directive=std::make_shared<Directive>();
    directive->setEscapeToken(std::string(1, DefaultEscapeToken));
    directive->setPlatformToken(defaultPlatformToken);
    return directive;
}
/**
 * @brief 解析Dockerfile内容
 * @details 主解析函数，处理Dockerfile内容并构建AST
 * @param rwc 包含Dockerfile内容的输入流
 * @return std::shared_ptr<Result> 包含AST、警告和解析结果的智能指针
 * @throws myerror 如果解析过程中出现错误
 */
std::shared_ptr<Result> Parse(std::stringstream& rwc) {
    logger->log_info("Start parsing Dockerfile");
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
                    logger->log_error("Unterminated heredoc: " + heredoc.Name);
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
        logger->log_error("Dockerfile stream read error");
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
/**
 * @brief 从行内容提取heredoc信息
 * @details 解析行内容中的heredoc标记并返回Heredoc对象集合
 * @param line 要解析的行内容
 * @return std::vector<Heredoc> 包含所有heredoc信息的集合
 * @throws myerror 如果heredoc格式无效
 */
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
/**
 * @brief 从行内容创建新节点
 * @details 解析Dockerfile行并创建对应的AST节点
 * @param line 要解析的行内容
 * @param directive 当前解析指令对象
 * @return std::shared_ptr<Node> 新创建的节点
 */
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

/**
 * @brief 去除字符串中的注释
 * @details 移除字符串中#符号及其后的所有内容
 * @param src 输入字符串
 * @return std::string 去除注释后的字符串
 */
std::string trimComments(const std::string& src) {
    static const std::regex tokenComment(R"(#.*?\r?$)", std::regex_constants::ECMAScript);
    std::string result = std::regex_replace(src, tokenComment, "");
    return result;
}
/**
 * @brief 去除字符串两端的空白字符
 * @details 移除字符串开头和结尾的空格、制表符等空白字符
 * @param src 输入字符串
 * @return std::string 去除空白后的字符串
 */
std::string trimWhitespace(const std::string& src) {
    std::string result = src;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    return result;
}
/**
 * @brief 去除字符串开头的空白字符
 * @details 仅移除字符串开头的空白字符，保留结尾空白
 * @param src 输入字符串
 * @return std::string 去除开头空白后的字符串
 */
std::string trimLeadingWhitespace(const std::string& src) {
    std::string result = src;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    return result;
}
/**
 * @brief 去除字符串开头的制表符
 * @details 仅移除字符串开头的制表符(\t)，保留其他空白字符
 * @param src 输入字符串
 * @return std::string 去除开头制表符后的字符串
 */
std::string trimLeadingTabs(const std::string& src) {
    std::string result = src;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
        return ch != '\t';
    }));
    return result;
}
/**
 * @brief 去除字符串末尾的换行符
 * @details 移除字符串末尾的\r和\n字符
 * @param src 输入字符串
 * @return std::string 去除换行符后的字符串
 */
std::string trimNewline(const std::string& src) {
    std::string result = src;
    result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
        return ch != '\r' && ch != '\n';
    }).base(), result.end());
    return result;
}
/**
 * @brief 去除续行字符
 * @details 根据指令对象中的续行正则表达式，去除行末的续行字符
 * @param line 要处理的字符串行
 * @param d 包含续行正则表达式的指令对象
 * @return std::tuple<std::string,bool> 返回处理后的字符串和是否到达行尾的标记
 */
std::tuple<std::string,bool> trimContinuationCharacter(std::string line, std::shared_ptr<Directive> d) {
    std::smatch match;
    if (std::regex_search(line, match, *d->lineContinuationRegex)) {
        // 去除续行字符
        std::string trimmedLine = std::regex_replace(line, *d->lineContinuationRegex, "");
        return std::make_tuple(trimmedLine, false);
    }
    return std::make_tuple(line, true);
}
/**
 * @brief 检查是否是空续行
 * @details 检查行是否只包含空白字符或注释(即空续行)
 * @param line 要检查的行
 * @return bool 如果是空续行返回true，否则返回false
 */
bool isEmptyContinuationLine(const std::string& line) {
    std::string trimmed = trimWhitespace(line);
    trimmed = trimComments(trimmed);
    return trimmed.empty();
}

/**
 * @brief 处理Dockerfile行
 * @details 处理Dockerfile行内容，包括去除空白、注释和解析指令
 * @param d 指令对象
 * @param token 要处理的行内容
 * @param stripLeftWhitespace 是否去除左侧空白字符
 * @return std::string 处理后的行内容
 * @throws myerror 如果解析指令时出错
 */
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
