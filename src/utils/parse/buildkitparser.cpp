#include "utils/parse/buildkitparser.h"
#include "utils/common/regexp.h"
#include "utils/cli/cobra/lex.h"
#include "utils/common/error.h"




// const auto DefaultEscapeToken = '\\';
std::shared_ptr<Heredoc> heredocFromMatch(const std::vector<std::string>& match) {
    if (match.empty()) {
        return nullptr;
    }

    unsigned int fd = 0;
    if (!match[1].empty()) {
        fd = std::stoul(match[1]);
    }

    bool chomp = (match[2] == "-");
    std::string rest = match[3];

    if (rest.size() == 0) {
        return nullptr;
    }

    auto shlex=NewLex('\\');
    shlex->SkipUnsetEnv=true;

    // Attempt to parse the heredoc both with and without quotes.
    shlex->RawQuotes=false;
    auto words = shlex->ProcessWords(rest,std::vector<std::string>{});
    if (words.size() != 1) {
        return nullptr;
    }
    if(words.size()!=1){
        return nullptr;
    }
    shlex->RawQuotes=true;
    auto wordsRaw = shlex->ProcessWords(rest,std::vector<std::string>{});
    if (wordsRaw.size() != words.size()) {
        throw myerror("internal lexing of heredoc produced inconsistent results: " + rest);
    }

    std::string word = words[0];
    std::string wordRaw = wordsRaw[0];
    int wordQuoteCount = std::count(word.begin(), word.end(), '\'') + std::count(word.begin(), word.end(), '"');
    int wordRawQuoteCount = std::count(wordRaw.begin(), wordRaw.end(), '\'') + std::count(wordRaw.begin(), wordRaw.end(), '"');

    bool expand = (wordQuoteCount == wordRawQuoteCount);
    auto re=std::make_shared<Heredoc>();
    re->Name=word;
    re->Expand=expand;
    re->Chomp=chomp;
    re->FileDescriptor=fd;
    return re;
    // return std::make_shared<Heredoc>(Heredoc{word, expand, chomp, fd});
}

std::shared_ptr<Heredoc> ParseHeredoc(const std::string& src){
    std::smatch match;
    if (std::regex_search(src, match, reHeredoc)) {
        return heredocFromMatch({match.begin(), match.end()});
    }

    return nullptr;
}