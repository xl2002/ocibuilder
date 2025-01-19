#if !defined(UTILS_PARSE_BUILDKITPARSER_H)
#define UTILS_PARSE_BUILDKITPARSER_H
#include <string> 
#include <vector>
#include <map>
#include <memory>
#include <tuple>
#include <regex>
extern std::regex reWhitespace;
extern std::regex reComment;
extern std::regex reHeredoc;
extern std::regex reLeadingTabs;
class Heredoc {
    public:
    std::string Name;
    std::string Content;
    uint32_t FileDescriptor=0;
    bool Expand=false;
    bool Chomp=false;
};
std::shared_ptr<Heredoc> ParseHeredoc(const std::string& src);
#endif // PARSE_BUILDKITPARSER_H
