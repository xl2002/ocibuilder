#if !defined(UTILS_PARSE_BUILDKITPARSER_H)
#define UTILS_PARSE_BUILDKITPARSER_H
#include <string> 
#include <vector>
#include <map>
#include <memory>
#include <tuple>
struct Heredoc {
    std::string Name;
    std::string Content;
    uint32_t FileDescriptor=0;
    bool Expand=false;
    bool Chomp=false;
};
std::shared_ptr<Heredoc> ParseHeredoc(const std::string& src);
#endif // PARSE_BUILDKITPARSER_H
