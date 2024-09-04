#include "regexp/regexp.h"


const bool precompile=false;
std::shared_ptr<Regexp> Delayed(const std::string& val) {
    auto re = std::make_shared<Regexp>(val);
    re->val = val;
    if (precompile) {
        re->MustCompile();
    }
    return re;
}

void Regexp::MustCompile(){
    regexp=std::make_shared<std::regex>(val);
}
void Regexp::compile() {
    if (precompile) {
        return;
    }
    std::call_once(once, [this]() {
        this->MustCompile();
    });
}
std::string Regexp::Expand(const std::string& dst, const std::string& tmpl, const std::string& src, const std::vector<int>& match) {
    compile();

    std::string result = dst;
    std::smatch sm;
    std::regex_search(src, sm, *regexp);

    if (!sm.empty()) {
        result.append(sm.format(tmpl));
    }
    return result;
}
std::string Regexp::Find(const std::string& b) {
    compile();

    std::smatch match;
    if (std::regex_search(b, match, *regexp)) {
        return match.str(0); // Return the first match
    }
    return ""; // Return an empty string if no match is found
}

std::vector<std::string> Regexp::FindAll(const std::string& b,int n) {
    compile();

    std::vector<std::string> results;
    std::sregex_iterator begin(b.begin(), b.end(), *regexp);
    std::sregex_iterator end;

    int count = 0;
    for (auto it = begin; it != end&& (n <= 0 || count < n); ++it) {
        results.push_back(it->str());
        ++count;
    }

    return results;
}
std::vector<std::pair<int, int>> Regexp::FindAllIndex(const std::string& b, int n) {
    compile();

    std::vector<std::pair<int, int>> results;
    std::sregex_iterator begin(b.begin(), b.end(), *regexp);
    std::sregex_iterator end;

    int count = 0;
    for (auto it = begin; it != end && (n <= 0 || count < n); ++it) {
        auto match = *it;
        int start = match.position();
        int end = start + match.length();
        results.emplace_back(start, end);
        ++count;
    }
    return results;
}
// Find all string submatches
std::vector<std::vector<std::string>> Regexp::FindAllSubmatch(const std::string& s, int n) {
    compile();

    std::vector<std::vector<std::string>> results;
    std::sregex_iterator begin(s.begin(), s.end(), *regexp);
    std::sregex_iterator end;

    int count = 0;
    for (auto it = begin; it != end && (n <= 0 || count < n); ++it) {
        std::smatch match = *it;
        std::vector<std::string> submatches;
        for (size_t i = 0; i < match.size(); ++i) {
            submatches.push_back(match[i].str());
        }
        results.push_back(submatches);
        ++count;
    }

    return results;
}
// Find all string submatch indices
std::vector<std::vector<int>> Regexp::FindAllSubmatchIndex(const std::string& s, int n) {
    compile();

    std::vector<std::vector<int>> results;
    std::sregex_iterator begin(s.begin(), s.end(), *regexp);
    std::sregex_iterator end;

    int count = 0;
    for (auto it = begin; it != end && (n <= 0 || count < n); ++it) {
        std::smatch match = *it;
        std::vector<int> indices;
        for (size_t i = 0; i < match.size(); ++i) {
            indices.push_back(match.position(i));
            indices.push_back(match.position(i) + match.length(i));
        }
        results.push_back(indices);
        ++count;
    }

    return results;
}
// Find the index of the first match
std::vector<int> Regexp::FindIndex(const std::string& s) {
    compile();
    
    std::smatch match;
    std::vector<int> loc;

    if (std::regex_search(s, match, *regexp)) {
        loc.push_back(match.position(0));
        loc.push_back(match.position(0) + match.length(0));
    }

    return loc;
}
// Find the index of the first match in a wide stream reader
std::vector<int> Regexp::FindReaderIndex(std::istream& r) {
    compile();
    
    std::string line;
    std::getline(r, line);
    
    std::smatch match;
    std::vector<int> loc;

    if (std::regex_search(line, match, *regexp)) {
        loc.push_back(match.position(0));
        loc.push_back(match.position(0) + match.length(0));
    }

    return loc;
}
// Find the index of all submatches in a wide stream reader
std::vector<int> Regexp::FindReaderSubmatchIndex(std::istream& r) {
    compile();

    std::string line;
    std::getline(r, line);

    std::smatch match;
    std::vector<int> indices;

    if (std::regex_search(line, match, *regexp)) {
        for (size_t i = 0; i < match.size(); ++i) {
            indices.push_back(match.position(i));
            indices.push_back(match.position(i) + match.length(i));
        }
    }

    return indices;
}
// Find submatches in a string
std::vector<std::string> Regexp::FindSubmatch(const std::string& s) {
    compile();

    std::smatch match;
    std::vector<std::string> submatches;

    if (std::regex_search(s, match, *regexp)) {
        for (size_t i = 0; i < match.size(); ++i) {
            submatches.push_back(match[i].str());
        }
    }

    return submatches;
}

// Find submatch indices in a string
std::vector<int> Regexp::FindSubmatchIndex(const std::string& s) {
    compile();

    std::smatch match;
    std::vector<int> indices;

    if (std::regex_search(s, match, *regexp)) {
        for (size_t i = 0; i < match.size(); ++i) {
            indices.push_back(match.position(i));
            indices.push_back(match.position(i) + match.length(i));
        }
    }

    return indices;
}
// LiteralPrefix function to get the literal prefix
std::tuple<std::string, bool> Regexp::LiteralPrefix(){
    compile();

    // Attempt to get a literal prefix from the regex pattern
    std::string prefix;
    bool complete = false;
    
    // Check if the pattern is a simple literal
    std::string pattern = "^" + val;
    std::regex literal_regex(pattern);

    // Example test string; adjust as needed
    std::string test_string = "a"; 
    std::smatch match;

    // Match the test string against the literal regex
    if (std::regex_search(test_string, match, literal_regex)) {
        prefix = test_string;
        complete = true;
    } else {
        // Extract literal prefix
        size_t pos = val.find_first_not_of("\\^$.|?*+()[]{}");
        if (pos != std::string::npos) {
            prefix = val.substr(0, pos);
        }
    }

    return std::make_tuple(prefix, complete);
}
// Match function to check if the regex matches the input string
bool Regexp::Match(const std::string& str) {
    compile();
    return std::regex_match(str, *regexp);
}
// MatchReader function to check if the regex matches the input stream
bool Regexp::MatchReader(std::istream& stream) {
    compile();
    std::string line;
    while (std::getline(stream, line)) {
        if (std::regex_search(line, *regexp)) {
            return true;
        }
    }
    return false;
}
// NumSubexp function to get the number of subexpressions in the regex
int Regexp::NumSubexp() {
    compile();
    // Create a regex object to check the number of subexpressions
    std::regex regex(val);
    // Create a regex iterator to count the number of capture groups
    std::smatch match;
    return std::distance(
        std::sregex_iterator(val.begin(), val.end(), regex),
        std::sregex_iterator()
    ) - 1; // Subtract 1 for the entire match
}
// Replace all occurrences of the regex in src with repl
std::string Regexp::ReplaceAll(const std::string& src, const std::string& repl) {
    compile();
    return std::regex_replace(src, *regexp, repl);
}

// Replace all matches with the result of the repl function
std::string Regexp::ReplaceAllFunc(const std::string& src, std::function<std::string(const std::string&)> repl) {
    compile();

    std::string result;
    auto begin = std::sregex_iterator(src.begin(), src.end(), *regexp);
    auto end = std::sregex_iterator();

    size_t lastPos = 0;
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        result.append(src.substr(lastPos, match.position() - lastPos));
        result.append(repl(match.str()));
        lastPos = match.position() + match.length();
    }

    result.append(src.substr(lastPos));
    return result;
}
// Replace all matches with a literal replacement
std::vector<uint8_t> Regexp::ReplaceAllLiteral(const std::vector<uint8_t>& src, const std::vector<uint8_t>& repl) {
    compile();

    // Convert src and repl from std::vector<uint8_t> to std::string
    std::string srcStr(src.begin(), src.end());
    std::string replStr(repl.begin(), repl.end());

    // Perform the replacement
    std::string resultStr = std::regex_replace(srcStr, *regexp, replStr);

    // Convert resultStr back to std::vector<uint8_t>
    return std::vector<uint8_t>(resultStr.begin(), resultStr.end());
}
// Split the string s by the regular expression into a vector of strings
std::vector<std::string> Regexp::Split(const std::string& s, int n) {
    compile();
    std::vector<std::string> result;
    auto regex = *regexp;
    std::sregex_token_iterator iter(s.begin(), s.end(), regex, -1);
    std::sregex_token_iterator end;

    for (int i = 0; i < n && iter != end; ++i, ++iter) {
        result.push_back(iter->str());
    }

    return result;
}
// Return the string representation of the regular expression
std::string Regexp::String() {
    compile();
    return val;  // std::regex does not have a method to return the pattern string directly
}

