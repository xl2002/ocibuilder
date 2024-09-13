#include "imagebuilder/shell_parser.h"
#include "cobra/error.h"
std::string ProcessWord(std::string word,std::vector<std::string>env){
    auto sw=std::make_shared<shellWord>();
    sw->word=word;
    sw->envs=env;
    sw->pos=0;
    std::string processedWord;
    // std::string err;
    sw->scanner.str(word);
    try {
        std::tie(processedWord,std::ignore) = sw->process();
    } catch (const myerror& e) {
        throw;
    }
    
    return processedWord;
}
std::vector<std::string> ProcessWords(std::string word,std::vector<std::string>env){
    auto sw=std::make_shared<shellWord>();
    sw->word=word;
    sw->envs=env;
    sw->pos=0;
    std::vector<std::string> words;
    // std::string err;
    sw->scanner.str(word);
    try {
        std::tie(std::ignore,words) = sw->process();
    } catch (const myerror& e) {
        throw;
    }
    
    return words;
}
// process函数，返回处理后的字符串、环境变量列表和错误信息
std::tuple<std::string, std::vector<std::string>> shellWord::process() {
    // 这里假设调用processStopOn方法，参数为EOF
    return processStopOn(EOF);  // 假设EOF对应boost::spirit的eof_p
}
// processStopOn函数，处理word并在遇到stopChar时停止
std::tuple<std::string, std::vector<std::string>> shellWord::processStopOn(int stopChar) {
    std::string result;
    wordsStruct words;

    // 字符映射到处理函数的表
    std::unordered_map<char, std::function<std::string()>> charFuncMapping = {
        {'\'', [this]() { return this->processSingleQuote(); }},
        {'"', [this]() { return this->processDoubleQuote(); }},
        {'$', [this]() { return this->processDollar(); }}
    };

    char ch;
    while (scanner.get(ch)) {
        if (stopChar != EOF && ch == stopChar) {
            return {result, words.getWords()};
        }

        if (charFuncMapping.find(ch) != charFuncMapping.end()) {
            // 调用对应字符的处理函数
            std::string tmp = charFuncMapping[ch]();
            result += tmp;

            if (ch == '$') {
                words.addString(tmp);
            } else {
                words.addRawString(tmp);
            }
        } else {
            // 非特殊字符，直接处理
            if (ch == '\\') {
                // '\' 转义
                if (!scanner.get(ch)) {
                    throw std::runtime_error("unexpected end of statement");
                }
                words.addRawChar(ch);
            } else {
                words.addChar(ch);
            }

            result += ch;
        }
    }

    if (stopChar != EOF) {
        throw std::runtime_error("unexpected end of statement while looking for matching " + std::string(1, stopChar));
    }

    return {result, words.getWords()};
}

// 获取处理后的所有单词
std::vector<std::string> wordsStruct::getWords() {
    if (!word.empty()) {
        words.push_back(word);  // 将当前处理的单词添加到words中

        // 防止重复调用时出现错误
        word.clear();           // 清空当前单词
        inWord = false;         // 重置inWord标志
    }
    return words;
}
// 添加字符，如果是空格且在单词中，则结束当前单词
void wordsStruct::addChar(char ch) {
    if (std::isspace(static_cast<unsigned char>(ch)) && inWord) {
        if (!word.empty()) {
            words.push_back(word);  // 将当前处理的单词添加到words中
            word.clear();           // 清空当前单词
            inWord = false;         // 重置inWord标志
        }
    } else if (!std::isspace(static_cast<unsigned char>(ch))) {
        addRawChar(ch);  // 非空格字符则添加到当前单词中
    }
}

// 添加非空格字符
void wordsStruct::addRawChar(char ch) {
    word += ch;          // 将字符添加到当前单词
    inWord = true;       // 标志处于单词中
}

// 将字符串拆分并添加为单词
void wordsStruct::addString(const std::string& str) {
    std::stringstream scan(str);  // 使用std::stringstream代替scanner
    char ch;
    while (scan.get(ch)) {        // 从字符串流中逐个获取字符
        addChar(ch);              // 逐个字符添加处理
    }
}

// 将整个字符串直接作为原始字符串添加
void wordsStruct::addRawString(const std::string& str) {
    word += str;  // 将字符串直接添加到当前单词
    inWord = true;
}

std::string shellWord::processSingleQuote() {
    // 所有单引号之间的字符原样保留
    // 注意，你不能转义 '
    std::string result;
    scanner.get(); // 跳过单引号

    char ch;
    while (scanner.get(ch)) {
        if (ch == '\'') {
            break;
        }
        result += ch;
    }

    if (scanner.eof()) {
        throw myerror("unexpected end of statement while looking for matching single-quote");
    }

    return result;
}

std::string shellWord::processDoubleQuote() {
    // 所有字符直到下一个 " 都原样保留，除了 $ 字符
    // 但可以用 \ 转义 "
    std::string result;
    scanner.get(); // 跳过双引号

    char ch;
    while (scanner.get(ch)) {
        if (ch == '"') {
            break;
        }
        if (ch == '$') {
            std::string tmp = processDollar();
            result += tmp;
        } else if (ch == '\\') {
            char nextCh;
            if (scanner.get(nextCh)) {
                if (nextCh == '"' || nextCh == '$' || nextCh == '\\') {
                    result += nextCh;
                } else {
                    scanner.putback(nextCh);
                }
            }
        } else {
            result += ch;
        }
    }

    if (scanner.eof()) {
        throw std::runtime_error("unexpected end of statement while looking for matching double-quote");
    }

    return result;
}

std::string shellWord::processDollar() {
    scanner.get(); // 跳过美元符号
    char ch = scanner.peek();
    if (ch == '{') {
        scanner.get(); // 跳过 {
        std::string name = processName();
        ch = scanner.peek();
        if (ch == '}') {
            // 正常的 ${xx} 形式
            scanner.get(); // 跳过 }
            return getEnv(name);
        }
        if (ch == ':') {
            // 特殊的 ${xx:...} 格式处理
            scanner.get(); // 跳过 :
            char modifier = scanner.get();

            std::string word;
            std::tie(word, std::ignore) = processStopOn('}');

            std::string newValue = getEnv(name);

            switch (modifier) {
                case '+':
                    if (!newValue.empty()) {
                        newValue = word;
                    }
                    return newValue;

                case '-':
                    if (newValue.empty()) {
                        newValue = word;
                    }
                    return newValue;

                case '?':
                    if (newValue.empty()) {
                        newValue = word;
                    }
                    if (newValue.empty()) {
                        throw std::runtime_error("Failed to process `" + word + "`: " + name + " is not allowed to be unset");
                    }
                    return newValue;

                default:
                    throw std::runtime_error("Unsupported modifier (" + std::string(1, modifier) + ") in substitution: " + word);
            }
        }
        throw std::runtime_error("Missing ':' in substitution: " + word);
    }

    // $xxx 形式
    std::string name = processName();
    if (name.empty()) {
        return "$";
    }
    return getEnv(name);
}
std::string shellWord::processName() {
    // 读取一个名称（字母数字或 _）
    // 如果以数字开头，则仅返回 $#
    std::string name;
    char ch;

    while (scanner.get(ch)) {
        if (name.empty() && std::isdigit(ch)) {
            return std::string(1, ch);
        }
        if (!std::isalnum(ch) && ch != '_') {
            scanner.putback(ch);
            break;
        }
        name += ch;
    }

    return name;
}

std::string shellWord::getEnv(const std::string& name) {
    for (const auto& env : envs) {
        size_t pos = env.find('=');
        if (pos == std::string::npos) {
            if (name == env) {
                // 如果没有等号，且 name 与 env 相等，则返回空字符串
                return "";
            }
            continue;
        }
        if (name == env.substr(0, pos)) {
            return env.substr(pos + 1);
        }
    }
    return "";
}
