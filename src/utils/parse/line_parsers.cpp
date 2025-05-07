/**
 * @file line_parsers.cpp
 * @brief 实现各种行解析功能的工具函数
 * @author [作者名]
 * @date 2025-04-09
 */

#include "utils/parse/line_parsers.h"
#include "utils/common/error.h"
/**
 * @brief 解析空白分隔的字符串列表
 * @param rest 要解析的输入字符串
 * @param d 指令对象指针
 * @return 包含解析结果节点和属性映射的元组
 *         - 第一个元素是解析后的节点树
 *         - 第二个元素是属性映射表
 */
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
/**
 * @brief 解析阶段枚举
 * 
 * 定义在解析字符串时的不同状态
 */
enum ParsePhase {
    InSpaces, ///< 查找单词开始状态
    InWord,   ///< 处理单词状态
    InQuote   ///< 处理引号内内容状态
};

/**
 * @brief 解析语句中的单词(空格分隔或引号包围的字符串)
 * @param rest 要解析的输入字符串
 * @param d 指令对象指针
 * @return 解析出的单词列表
 * @note 引号会保留在结果中，稍后会在processWords()中被去除
 */
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
/**
 * @brief 创建新的键值对节点
 * @param key 键字符串
 * @param value 值字符串
 * @return 新创建的节点指针
 */
std::shared_ptr<Node> newKeyValueNode(const std::string& key, const std::string& value) {
    auto node = std::make_shared<Node>();
    node->Value = key;
    node->Next = std::make_shared<Node>();
    node->Next->Value = value;
    return node;
}
/**
 * @brief 将键值对节点追加到列表中
 * @param node 要追加的节点
 * @param rootNode 列表根节点
 * @param prevNode 前一个节点
 * @return 包含更新后的根节点和前一个节点的元组
 */
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
/**
 * @brief 解析名称-值对
 * @param rest 要解析的输入字符串
 * @param key 键名
 * @param d 指令对象指针
 * @return 解析后的节点树
 * @throws myerror 当输入格式不正确时抛出异常
 */
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

/**
 * @brief 解析LABEL指令
 * @param rest 要解析的输入字符串
 * @param d 指令对象指针
 * @return 包含解析结果节点和属性映射的元组
 * @throws myerror 当解析失败时抛出异常
 */
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
/**
 * @brief 解析ENV指令
 * @param rest 要解析的输入字符串
 * @param d 指令对象指针
 * @return 包含解析结果节点和属性映射的元组
 * @throws myerror 当解析失败时抛出异常
 */
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
/**
 * @brief 去除字符串前导和尾部空白
 * @param str 输入字符串
 * @return 去除空白后的字符串
 */
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

/**
 * @brief 从字符串中提取下一个JSON字符串
 * @param str 输入字符串
 * @param pos 当前解析位置(会被更新)
 * @return 提取出的字符串
 * @throws myerror 当字符串未正确终止时抛出异常
 */
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

/**
 * @brief 解析JSON对象
 * @param str 包含JSON的字符串
 * @param pos 当前解析位置(会被更新)
 * @return 包含解析结果节点、属性映射和异常指针的元组
 * @throws myerror 当JSON格式不正确时抛出异常
 */
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>, std::exception_ptr> parseJSONObject(
    const std::string& str,
    size_t& pos
);

/**
 * @brief 解析JSON数组
 * @param str 包含JSON的字符串
 * @param pos 当前解析位置(会被更新)
 * @return 包含解析结果节点、属性映射和异常指针的元组
 * @throws myerror 当JSON格式不正确时抛出异常
 */
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>, std::exception_ptr> parseJSONArray(
    const std::string& str,
    size_t& pos
);

/**
 * @brief 解析JSON数组或对象
 * @param rest 要解析的输入字符串
 * @param d 指令对象指针
 * @return 包含解析结果节点、属性映射和异常指针的元组
 * @throws myerror 当JSON格式不正确时抛出异常
 */
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

/**
 * @brief 解析JSON数组
 * @param str 包含JSON数组的字符串
 * @param pos 当前解析位置(会被更新)
 * @return 包含解析结果节点、属性映射和异常指针的元组
 *         - 第一个元素是解析后的节点链表
 *         - 第二个元素是属性映射表(json=true表示JSON格式)
 *         - 第三个元素是异常指针(解析成功时为nullptr)
 * @throws myerror 当JSON格式不正确时抛出异常
 */
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
/**
 * @brief 尝试将输入解析为JSON列表，失败则回退到空白分隔解析
 * @param rest 要解析的输入字符串
 * @param d 指令对象指针
 * @return 包含解析结果节点和属性映射的元组
 * @throws myerror 当JSON数组包含非字符串元素时抛出异常
 */
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
/**
 * @brief 尝试将输入解析为JSON，失败则作为普通字符串处理
 * @param rest 要解析的输入字符串
 * @param d 指令对象指针
 * @return 包含解析结果节点和属性映射的元组
 * @throws myerror 当JSON数组包含非字符串元素时抛出异常
 */
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
/**
 * @brief 将输入作为简单字符串解析
 * @param rest 要解析的输入字符串
 * @param d 指令对象指针
 * @return 包含解析结果节点和属性映射的元组
 */
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
/**
 * @brief 忽略输入的特殊解析器
 * @param rest 要解析的输入字符串(被忽略)
 * @param d 指令对象指针
 * @return 包含空节点和空属性映射的元组
 */
std::tuple<std::shared_ptr<Node>, std::map<std::string, bool>> parseIgnore(
    const std::string& rest,
    const std::shared_ptr<Directive> d
) {
    return std::make_tuple(nullptr, std::map<std::string, bool>());
}
