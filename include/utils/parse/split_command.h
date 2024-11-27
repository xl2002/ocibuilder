#if !defined(PAARSE_SPLIT_COMMAND_H)
#define PAARSE_SPLIT_COMMAND_H
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <sstream>
#include <regex>
std::tuple<std::string, std::vector<std::string>> extractBuilderFlags(const std::string& line);
std::tuple<std::string, std::vector<std::string>, std::string> splitCommand(const std::string& line);

#endif // PAARSE_SPLIT_COMMAND_H
