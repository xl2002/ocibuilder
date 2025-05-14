/**
 * @file buildkitparser.cpp
 * @brief Implementation of BuildKit heredoc parsing functionality
 * 
 * This file contains functions for parsing heredoc syntax in BuildKit configuration files.
 * It handles both quoted and unquoted heredoc delimiters with optional chomping.
 */

#include "utils/parse/buildkitparser.h"
#include "utils/common/regexp.h"
#include "utils/cli/cobra/lex.h"
#include "utils/common/error.h"
#include "utils/logger/ProcessSafeLogger.h"




// const auto DefaultEscapeToken = '\\';
/**
 * @brief Creates a Heredoc object from regex match results
 * 
 * @param match Regex match results containing heredoc components:
 *              [0] - full match
 *              [1] - optional file descriptor
 *              [2] - optional chomp indicator (-)
 *              [3] - heredoc delimiter
 * @return std::shared_ptr<Heredoc> Parsed heredoc object or nullptr if invalid
 */
std::shared_ptr<Heredoc> heredocFromMatch(const std::vector<std::string>& match) {
    logger->log_info("Starting heredoc parsing from match");
    if (match.empty()) {
        logger->log_error("Empty match vector provided to heredocFromMatch");
        return nullptr;
    }

    unsigned int fd = 0;
    if (!match[1].empty()) {
        fd = std::stoul(match[1]);
    }

    bool chomp = (match[2] == "-");
    std::string rest = match[3];

    if (rest.size() == 0) {
        logger->log_error("Empty heredoc delimiter found");
        return nullptr;
    }

    auto shlex=NewLex('\\');
    shlex->SkipUnsetEnv=true;

    // Attempt to parse the heredoc both with and without quotes.
    shlex->RawQuotes=false;
    auto words = shlex->ProcessWords(rest,std::vector<std::string>{});
    if (words.size() != 1) {
        logger->log_error("Invalid heredoc delimiter - multiple words found");
        return nullptr;
    }
    if(words.size()!=1){
        logger->log_error("Invalid heredoc delimiter - words size mismatch");
        return nullptr;
    }
    shlex->RawQuotes=true;
    auto wordsRaw = shlex->ProcessWords(rest,std::vector<std::string>{});
    if (wordsRaw.size() != words.size()) {
        logger->log_error("Lexing inconsistency detected in heredoc: " + rest);
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

/**
 * @brief Parses a heredoc declaration from source string
 * 
 * @param src Input string containing heredoc declaration
 * @return std::shared_ptr<Heredoc> Parsed heredoc object or nullptr if no match
 */
std::shared_ptr<Heredoc> ParseHeredoc(const std::string& src){
    logger->log_info("Starting heredoc parsing for source string");
    std::smatch match;
    if (std::regex_search(src, match, reHeredoc)) {
        return heredocFromMatch({match.begin(), match.end()});
    }

    return nullptr;
}
