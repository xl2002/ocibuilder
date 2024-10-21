#include "parse/split_command.h"
#include "go/string.h"
#include "parse/dockerfileparse.h"
#include "cobra/error.h"
// splitCommand takes a single line of text and parses out the cmd and args,
// which are used for dispatching to more exact parsing functions.
std::tuple<std::string, std::vector<std::string>, std::string> splitCommand(const std::string& line) {
    std::string args;
    std::vector<std::string> flags;

    // Make sure we get the same results irrespective of leading/trailing spaces
    std::vector<std::string> cmdline =tokenWhitespace->Split(TrimSpace(line), 2);
    std::string cmd = toLower(cmdline[0]);

    // std::error_code ec;
    if (cmdline.size() == 2) {
        try
        {
            std::tie(args, flags) = extractBuilderFlags(cmdline[1]);
            // return std::make_tuple(cmd, flags, TrimSpace(args));
        }
        catch(const myerror& e)
        {
            throw;
        }
    }

    return std::make_tuple(cmd, flags, TrimSpace(args));
}

std::tuple<std::string, std::vector<std::string>> extractBuilderFlags(const std::string& line) {
    const int inSpaces = 0;
    const int inWord = 1;
    const int inQuote = 2;

    std::vector<std::string> words;
    int phase = inSpaces;
    std::string word;
    char quote = '\0';
    bool blankOK = false;
    char ch;
    std::string remainingLine = line;

    for (size_t pos = 0; pos <= line.length(); ++pos) {
        if (pos != line.length()) {
            ch = line[pos];
        }

        if (phase == inSpaces) { // Looking for start of word
            if (pos == line.length()) { // end of input
                break;
            }
            if (std::isspace(ch)) { // skip spaces
                continue;
            }

            // Only keep going if the next word starts with --
            if (ch != '-' || pos + 1 == line.length() || line[pos + 1] != '-') {
                remainingLine = line.substr(pos);
                return std::make_tuple(remainingLine, words);
            }

            phase = inWord; // found something with "--", fall through
        }
        if ((phase == inWord || phase == inQuote) && (pos == line.length())) {
            if (word != "--" && (blankOK || !word.empty())) {
                words.push_back(word);
            }
            break;
        }
        if (phase == inWord) {
            if (std::isspace(ch)) {
                phase = inSpaces;
                if (word == "--") {
                    remainingLine = line.substr(pos);
                    return std::make_tuple(remainingLine, words);
                }
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
                phase = inQuote;
                continue;
            }
            if (ch == '\\') {
                if (pos + 1 == line.length()) {
                    continue; // just skip \ at end
                }
                pos++;
                ch = line[pos];
            }
            word += ch;
            continue;
        }
        if (phase == inQuote) {
            if (ch == quote) {
                phase = inWord;
                continue;
            }
            if (ch == '\\') {
                if (pos + 1 == line.length()) {
                    phase = inWord;
                    continue; // just skip \ at end
                }
                pos++;
                ch = line[pos];
            }
            word += ch;
        }
    }

    return std::make_tuple("", words);
}