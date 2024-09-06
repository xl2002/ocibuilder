#include "shell/lex.h"
std::shared_ptr<Lex> NewLex(const char& escapeToken){
    auto ret=std::make_shared<Lex>();
    ret->escapeToken=escapeToken;
    return ret;
}

std::map<std::string, std::string> BuildEnvs(const std::vector<std::string>& env) {
    std::map<std::string, std::string> envs;

    for (const auto& e : env) {
        size_t i = e.find('=');

        if (i == std::string::npos) {
            envs[e] = "";
        } else {
            std::string k = e.substr(0, i);
            std::string v = e.substr(i + 1);

            // 如果key已经存在，覆盖其值
            envs[k] = v;
        }
    }

    return envs;
}
std::vector<std::string>Lex::ProcessWords(std::string word,std::vector<std::string> env){
    std::string w;
    std::vector<std::string> words;
    std::tie(w,words)=process(word,BuildEnvs(env));
    return words;
}
std::tuple<std::string, std::vector<std::string>> Lex::process(std::string word, std::map<std::string, std::string> env) {
    auto sw=init(word,env);
    return sw->process(word);
}
std::shared_ptr<shellWord_lex> Lex::init(std::string word, std::map<std::string, std::string> env) {
    auto sw=std::make_shared<shellWord_lex>();
    sw->envs=env;
    sw->escapeToken=escapeToken;
    sw->rawEscapes=RawEscapes;
    sw->rawQuotes=RawQuotes;
    sw->skipUnsetEnv=SkipUnsetEnv;
    sw->skipProcessQuotes=SkipProcessQuotes;
    sw->matches=std::map<std::string, std::string>();
    return sw;
}

std::tuple<std::string,std::vector<std::string>> shellWord_lex::process(std::string source){
    std::string word;
    std::vector<std::string> words;
    try
    {
        std::tie(word, words) = processStopOn(EOF);
        return std::make_tuple(word, words);
    }
    catch(const myerror& e)
    {
        throw myerror(std::string(e.what())+"\nfailed to process "+source);
    }
}
std::tuple<std::string, std::vector<std::string>> shellWord_lex::processStopOn(char stopChar) {
    std::ostringstream result;
    wordsStruct_lex words;

    // 映射字符到对应的处理函数
    std::map<char, std::function<std::string()>> charFuncMapping = {
        {'$',[this](){return processDollar();}}
    };

    if (!skipProcessQuotes) {
        charFuncMapping['\''] = [this](){return processSingleQuote();};
        charFuncMapping['"'] = [this](){return processDoubleQuote();};
    }

    while (!ss.eof()) {
        char ch = ss.peek();

        if (stopChar != '\0' && ch == stopChar) {
            ss.get(); // Consume the stopChar
            return std::make_tuple(result.str(), words.getWords());
        }

        auto it = charFuncMapping.find(ch);
        if (it != charFuncMapping.end()) {
            // Call special processing function for certain chars
            std::string tmp = it->second();
            result << tmp;

            if (ch == '$') {
                words.addString(tmp);
            } else {
                words.addRawString(tmp);
            }
        } else {
            // Not special, just add it to the result
            ch = ss.get();

            if (ch == escapeToken) {
                if (rawEscapes) {
                    words.addRawChar(ch);
                    result << ch;
                }

                if (!ss.eof()) {
                    ch = ss.get();
                    if (ss.eof()) {
                        break;
                    }

                    words.addRawChar(ch);
                    result << ch;
                }
            } else {
                words.addChar(ch);
                result << ch;
            }
        }
    }

    if (stopChar != '\0') {
        throw std::runtime_error("unexpected end of statement while looking for matching character");
    }

    return std::make_tuple(result.str(), words.getWords());
}
std::string shellWord_lex::processDollar() {
    ss.get(); // Consume $

    if (ss.peek() != '{') {
        std::string name = processName();
        if (name.empty()) {
            return "$";
        }
        auto it = envs.find(name);
        if (it == envs.end() && skipUnsetEnv) {
            return "$" + name;
        }
        return (it != envs.end()) ? it->second : "";
    }

    ss.get(); // Consume {
    char ch = ss.get();
    std::string name = processName();
    std::string chs(1, ch);
    bool nullIsUnset = false;

    switch (ch) {
        case '}':
            {
                auto it = envs.find(name);
                if (it == envs.end() && skipUnsetEnv) {
                    return "${" + name + "}";
                }
                return (it != envs.end()) ? it->second : "";
            }
        case ':':
            nullIsUnset = true;
            ch = ss.get();
            chs += ch;
            [[fallthrough]]; // C++17
        case '+':
        case '-':
        case '?':
            {   
                std::string word;
                std::vector<std::string> words;
                std::tie(word, words)= processStopOn('}');
                auto it = envs.find(name);
                if (skipUnsetEnv && it == envs.end()) {
                    return "${" + name + chs + word + "}";
                }

                if (ch == '-') {
                    if (it == envs.end() || (nullIsUnset && it->second.empty())) {
                        return word;
                    }
                    return (it != envs.end()) ? it->second : "";
                } else if (ch == '+') {
                    if (it == envs.end() || (nullIsUnset && it->second.empty())) {
                        return "";
                    }
                    return word;
                } else if (ch == '?') {
                    if (it == envs.end()) {
                        std::string message = "is not allowed to be unset";
                        if (!word.empty()) {
                            message = word;
                        }
                        throw std::runtime_error(name + ": " + message);
                    }
                    if (nullIsUnset && it->second.empty()) {
                        std::string message = "is not allowed to be empty";
                        if (!word.empty()) {
                            message = word;
                        }
                        throw std::runtime_error(name + ": " + message);
                    }
                    return it->second;
                } else {
                    throw std::runtime_error("unsupported modifier (" + chs + ") in substitution");
                }
            }
        default:
            throw std::runtime_error("unsupported modifier (" + chs + ") in substitution");
    }
}
// 检查字符是否为特殊参数
bool isSpecialParam(char ch) {
    switch (ch) {
        case '@':
        case '*':
        case '#':
        case '?':
        case '-':
        case '$':
        case '!':
        case '0':
            return true;
        default:
            return false;
    }
}
std::string shellWord_lex::processName() {
    std::string name;
    char ch;

    while (ss.peek() != EOF) {
        ch = ss.peek();

        if (name.empty() && std::isdigit(ch)) {
            // Read numeric characters
            while (ss.peek() != EOF && std::isdigit(ss.peek())) {
                name += ss.get();
            }
            return name;
        }
        if (name.empty() && isSpecialParam(ch)) {
            name += ss.get();
            return name;
        }
        if (!std::isalnum(ch) && ch != '_') {
            break;
        }
        name += ss.get();
    }

    return name;
}
// 处理单引号内的内容
std::string shellWord_lex::processSingleQuote() {
    std::ostringstream result;
    char ch;

    // 读取单引号字符（起始字符）
    ss.get(ch);
    if (rawQuotes) {
        result << ch;  // 如果 rawQuotes 为 true，保留单引号
    }

    while (ss.get(ch)) {
        if (ch == '\'') {  // 遇到结束单引号
            if (rawQuotes) {
                result << ch;  // 如果 rawQuotes 为 true，保留结束单引号
            }
            return result.str();
        }
        result << ch;  // 其他字符直接追加到结果中
    }

    // 如果到达这里，表示遇到了文件结束而没有找到结束单引号
    throw myerror("unexpected end of statement while looking for matching single-quote");
}

std::string shellWord_lex::processDoubleQuote() {
    std::ostringstream result;
    char ch;

    // 读取起始的双引号
    ss.get(ch);
    if (rawQuotes) {
        result << ch;
    }

    while (ss.get(ch)) {
        if (ch == '"') {
            if (rawQuotes) {
                result << ch;
            }
            return result.str();
        }
        if (ch == '$') {
            std::string value;
            try {
                value = processDollar();
            } catch (const std::exception& e) {
                throw myerror("Error processing dollar sign: " + std::string(e.what()));
            }
            result << value;
        } else if (ch == escapeToken) {
            if (rawEscapes) {
                result << ch;
            }
            char nextChar = ss.peek();
            if (nextChar == '"' || nextChar == '$' || nextChar == escapeToken) {
                ss.get(ch); // consume escape token
            }
            // otherwise, escape sequence is treated as literal
            result << ch;
        } else {
            result << ch;
        }
    }

    throw myerror("unexpected end of statement while looking for matching double-quote");
}
// 添加单个字符到当前单词中
void wordsStruct_lex::addChar(char ch) {
    if (std::isspace(ch) && inWord) {
        // 遇到空格时，如果正在处理一个单词，则将当前单词添加到列表中
        if (!word.empty()) {
            words.push_back(word);
            word.clear();
            inWord = false;
        }
    } else if (!std::isspace(ch)) {
        // 遇到非空格字符时，调用 addRawChar 处理字符
        addRawChar(ch);
    }
}
void wordsStruct_lex:: addRawChar(char ch) {
    word+=ch;
    inWord = true;
}

void wordsStruct_lex::addString(const std::string& str) {
    for(auto& ch:str){
        addChar(ch);
    }
}
void wordsStruct_lex::addRawString(const std::string& str) {
    word+=str;
    inWord = true;
}
std::vector<std::string> wordsStruct_lex::getWords(){
    if(word.size()>0){
        words.push_back(word);
        word="";
        inWord=false;
    }
    return words;
}