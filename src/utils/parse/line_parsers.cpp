#include "utils/parse/line_parsers.h"
#include "utils/common/error.h"
// #include <nlohmann/json.hpp>
// 函数用于解析空白分隔的字符串列表
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>> 
parseStringsWhitespaceDelimited(const std::string& rest, std::shared_ptr<Directive>d) {
    if (rest.empty()) {
        return std::make_tuple(nullptr, std::map<std::string, bool>());
    }

    auto rootnode = std::make_shared<Node>();
    auto node = rootnode;
    auto prevnode = node;

    // 使用正则表达式进行拆分
    std::sregex_token_iterator iter(rest.begin(), rest.end(), *tokenWhitespace->regexp, -1);
    std::sregex_token_iterator end;
    for (; iter != end; ++iter) {
        prevnode = node;
        node->Value = *iter;
        node->Next = std::make_shared<Node>();
        node = node->Next;
    }

    // 处理正则表达式拆分后最后一个节点为空字符串的情况
    prevnode->Next = nullptr;

    return std::make_tuple(rootnode, std::map<std::string, bool>());
}
enum ParsePhase {
    InSpaces, // Looking for start of a word
    InWord,
    InQuote
};

// Helper function to parse words (i.e., space-delimited or quoted strings) in a statement.
// The quotes are preserved as part of this function and they are stripped later
// as part of processWords().
std::vector<std::string> parseWords(const std::string& rest, std::shared_ptr<Directive> d) {
    std::vector<std::string> words;
    ParsePhase phase = InSpaces;
    std::string word;
    char quote = '\0';
    bool blankOK = false;

    for (size_t pos = 0; pos <= rest.size(); ++pos) {
        char ch = (pos < rest.size()) ? rest[pos] : '\0';

        if (phase == InSpaces) { // Looking for start of word
            if (pos == rest.size()) { // end of input
                break;
            }
            if (std::isspace(ch, std::locale::classic())) { // skip spaces
                continue;
            }
            phase = InWord; // found it, fall through
        }

        if ((phase == InWord || phase == InQuote) && (pos == rest.size())) {
            if (blankOK || !word.empty()) {
                words.push_back(word);
            }
            break;
        }

        if (phase == InWord) {
            if (std::isspace(ch, std::locale::classic())) {
                phase = InSpaces;
                if (blankOK || !word.empty()) {
                    words.push_back(word);
                }
                word.clear();
                blankOK = false;
                continue;
            }
            if (ch == '\'' || ch == '"') {
                quote = ch;
                blankOK = true;
                phase = InQuote;
            } else if (ch == d->escapeToken) {
                if (pos + 1 == rest.size()) {
                    continue; // just skip an escape token at end of line
                }
                word += ch;
                ch = rest[++pos];
            }
            word += ch;
            continue;
        }

        if (phase == InQuote) {
            if (ch == quote) {
                phase = InWord;
            } else if (ch == d->escapeToken && quote != '\'') {
                if (pos + 1 == rest.size()) {
                    phase = InWord;
                    continue; // just skip the escape token at end
                }
                word += ch;
                ch = rest[++pos];
            }
            word += ch;
        }
    }

    return words;
}
// Function to create a new key-value node
std::shared_ptr<Node> newKeyValueNode(const std::string& key, const std::string& value) {
    auto node = std::make_shared<Node>();
    node->Value = key;
    node->Next = std::make_shared<Node>();
    node->Next->Value = value;
    return node;
}
// Function to append a key-value node to the list
std::tuple<std::shared_ptr<Node>, std::shared_ptr<Node>> appendKeyValueNode(
    std::shared_ptr<Node> node,
    std::shared_ptr<Node> rootNode,
    std::shared_ptr<Node> prevNode
) {
    if (!rootNode) {
        rootNode = node;
    }
    if (prevNode) {
        prevNode->Next = node;
    }

    prevNode = node->Next;
    return std::make_tuple(rootNode, prevNode);
}
std::shared_ptr<Node> parseNameVal(const std::string& rest, const std::string& key, std::shared_ptr<Directive> d) {
    // 使用 parseWords 函数解析字符串 rest，返回单词列表
    std::vector<std::string> words = parseWords(rest, d);
    if (words.empty()) {
        return nullptr;
    }

    // 处理旧格式 (KEY name value)
    if (words[0].find('=') == std::string::npos) {
        std::vector<std::string> parts;
        std::regex ws_re("\\s+");
        std::sregex_token_iterator iter(rest.begin(), rest.end(), ws_re, -1);
        std::sregex_token_iterator end;

        for (; iter != end; ++iter) {
            parts.push_back(*iter);
        }

        if (parts.size() < 2) {
            throw myerror(key + " must have two arguments");
        }
        return newKeyValueNode(parts[0], parts[1]);
    }

    std::shared_ptr<Node> rootNode = nullptr;
    std::shared_ptr<Node> prevNode = nullptr;
    for (const auto& word : words) {
        if (word.find('=') == std::string::npos) {
            throw myerror("Syntax error - can't find = in \"" + word + "\". Must be of the form: name=value");
        }

        size_t pos = word.find('=');
        std::string name = word.substr(0, pos);
        std::string value = word.substr(pos + 1);

        auto node = newKeyValueNode(name, value);
        std::tie(rootNode, prevNode) = appendKeyValueNode(node, rootNode, prevNode);
    }

    return rootNode;
}
// const std::string commandLabel = "LABEL";
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>>
parseLabel(const std::string& rest, std::shared_ptr<Directive> d) {
    // 假设 parseNameVal 是一个已定义的函数，返回一个 tuple
    try
    {
        auto result = parseNameVal(rest, "LABEL", d);
        // 返回的结果和 Go 语言类似，只是最后一个值设为 nullptr
        return std::make_tuple(result, std::map<std::string, bool>());
    }
    catch(const myerror& e)
    {
        throw;
    }
}
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>>
parseEnv(const std::string& rest, std::shared_ptr<Directive> d) {
    // 假设 parseNameVal 是一个已定义的函数，返回一个 tuple
    try
    {
        auto result = parseNameVal(rest, "ENV", d);
        // 返回的结果和 Go 语言类似，只是最后一个值设为 nullptr
        return std::make_tuple(result, std::map<std::string, bool>());
    }
    catch(const myerror& e)
    {
        throw;
    }
}
// 去除字符串前导和尾部空白
std::string trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();

    while (start < end && std::isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        --end;
    }

    return str.substr(start, end - start);
}

// 从字符串中提取下一个 JSON 字符串
std::string extractNextString(const std::string& str, size_t& pos) {
    std::string result;
    bool inQuotes = false;

    while (pos < str.size()) {
        char ch = str[pos++];

        if (ch == '"') {
            if (inQuotes) {
                return result;
            } else {
                inQuotes = true;
            }
        } else if (ch == '\\') {
            if (pos < str.size()) {
                result += str[pos++];
            }
        } else {
            result += ch;
        }
    }

    throw myerror("Unterminated string in JSON");
}

// 解析 JSON 对象
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>, std::exception_ptr> parseJSONObject(
    const std::string& str,
    size_t& pos
);

// 解析 JSON 数组
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>, std::exception_ptr> parseJSONArray(
    const std::string& str,
    size_t& pos
);

// 解析 JSON 数组或对象
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>, std::exception_ptr> parseJSON(
    const std::string& rest,
    const std::shared_ptr<Directive> d
) {
    std::string trimmed_rest = trim(rest);

    if (trimmed_rest.empty()) {
        return std::make_tuple(nullptr, std::map<std::string, bool>{}, std::make_exception_ptr(
            myerror("Error parsing \"" + rest + "\" as a JSON array or object")
        ));
    }

    size_t pos = 0;
    if (trimmed_rest[pos] == '[') {
        return parseJSONArray(trimmed_rest, pos);
    } else if (trimmed_rest[pos] == '{') {
        return parseJSONObject(trimmed_rest, pos);
    } else {
        return std::make_tuple(nullptr, std::map<std::string, bool>{}, std::make_exception_ptr(
            myerror("Error parsing \"" + rest + "\" as a JSON array or object")
        ));
    }
}

// 解析 JSON 对象
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>, std::exception_ptr> parseJSONObject(
    const std::string& str,
    size_t& pos
) {
    if (str[pos] != '{') {
        throw myerror("Expected '{' at the start of JSON object");
    }
    ++pos;  // Skip the opening '{'

    auto top = std::shared_ptr<Node>(nullptr);
    auto prev = std::shared_ptr<Node>(nullptr);

    while (pos < str.size() && str[pos] != '}') {
        std::string key;
        std::string value;

        // Extract key
        if (str[pos] != '"') {
            throw myerror("Expected '\"' at the start of JSON key");
        }
        key = extractNextString(str, pos);
        if (str[pos] != ':') {
            throw myerror("Expected ':' after JSON key");
        }
        ++pos;  // Skip ':'

        // Extract value
        if (str[pos] == '"') {
            value = extractNextString(str, pos);
        } else if (str[pos] == '{') {
            auto result = parseJSONObject(str, pos);
            if (std::get<0>(result)) {
                value = "[object]";
            } else {
                return result;
            }
        } else if (str[pos] == '[') {
            auto result = parseJSONArray(str, pos);
            if (std::get<0>(result)) {
                value = "[array]";
            } else {
                return result;
            }
        } else {
            throw myerror("Unexpected character in JSON value");
        }

        auto node = std::make_shared<Node>();
        node->Value = key + "=" + value;

        if (prev == nullptr) {
            top = node;
        } else {
            prev->Next = node;
        }
        prev = node;

        // Skip optional comma
        if (pos < str.size() && str[pos] == ',') {
            ++pos;
        }
    }

    if (str[pos] != '}') {
        throw myerror("Expected '}' at the end of JSON object");
    }
    ++pos;  // Skip the closing '}'

    return std::make_tuple(top, std::map<std::string, bool>{{"json", true}}, nullptr);
}

// 解析 JSON 数组
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>, std::exception_ptr> parseJSONArray(
    const std::string& str,
    size_t& pos
) {
    if (str[pos] != '[') {
        throw myerror("Expected '[' at the start of JSON array");
    }
    ++pos;  // Skip the opening '['

    auto top = std::shared_ptr<Node>(nullptr);
    auto prev = std::shared_ptr<Node>(nullptr);

    while (pos < str.size() && str[pos] != ']') {
        std::string value;

        // Extract value
        if (str[pos] == '"') {
            value = extractNextString(str, pos);
        } else if (str[pos] == '{') {
            auto result = parseJSONObject(str, pos);
            if (std::get<0>(result)) {
                value = "[object]";
            } else {
                return result;
            }
        } else if (str[pos] == '[') {
            auto result = parseJSONArray(str, pos);
            if (std::get<0>(result)) {
                value = "[array]";
            } else {
                return result;
            }
        } else {
            throw myerror("Unexpected character in JSON value");
        }

        auto node = std::make_shared<Node>();
        node->Value = value;

        if (prev == nullptr) {
            top = node;
        } else {
            prev->Next = node;
        }
        prev = node;

        // Skip optional comma
        if (pos < str.size() && str[pos] == ',') {
            ++pos;
        }
    }

    if (str[pos] != ']') {
        throw myerror("Expected ']' at the end of JSON array");
    }
    ++pos;  // Skip the closing ']'

    return std::make_tuple(top, std::map<std::string, bool>{{"json", true}}, nullptr);
}
// parseMaybeJSONToList 实现
// auto errDockerfileNotStringArray=std::make_exception_ptr(
//             myerror("when using JSON array syntax, arrays must be comprised of strings only")
//         );
// Helper function to split a string by whitespace

std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>> parseMaybeJSONToList(
    const std::string& rest,
    const std::shared_ptr<Directive> d
) {
    std::shared_ptr<Node> node;
    std::map<std::string, bool> attrs;
    std::exception_ptr err;
    std::tie(node, attrs, err) = parseJSON(rest, d);

    if (err == nullptr) {
        return std::make_tuple(node, attrs);
    }
    try{
        std::rethrow_exception(err);
    }
    catch(const myerror& e){
        if(std::string(e.what())=="when using JSON array syntax, arrays must be comprised of strings only")
        throw;
    }
    return parseStringsWhitespaceDelimited(rest, d);
}
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>> parseMaybeJSON(
    const std::string& rest,
    const std::shared_ptr<Directive> d
) {
    if(rest==""){
        return std::make_tuple(nullptr, std::map<std::string, bool>());
    }
    std::shared_ptr<Node> node;
    std::map<std::string, bool> attrs;
    std::exception_ptr err;
    std::tie(node, attrs, err) = parseJSON(rest, d);
    if (err == nullptr) {
        return std::make_tuple(node, attrs);
    }
    try{
        std::rethrow_exception(err);
    }
    catch(const myerror& e){
        if(std::string(e.what())=="when using JSON array syntax, arrays must be comprised of strings only")
        throw;
    }
    node=std::make_shared<Node>();
    node->Value=rest;
    return std::make_tuple(node, std::map<std::string, bool>());
}
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>> parseString(
    const std::string& rest,
    const std::shared_ptr<Directive> d
){
    if(rest==""){
        return std::make_tuple(nullptr, std::map<std::string, bool>());
    }
    auto node=std::make_shared<Node>();
    node->Value=rest;
    return std::make_tuple(node, std::map<std::string, bool>());
}
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>> parseIgnore(
    const std::string& rest,
    const std::shared_ptr<Directive> d
) {
    return std::make_tuple(nullptr, std::map<std::string, bool>());
}