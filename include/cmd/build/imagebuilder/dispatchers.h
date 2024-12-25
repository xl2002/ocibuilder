#if !defined(CMD_BUILD_IMAGEBUILDER_DISPATCHERS_H)
#define CMD_BUILD_IMAGEBUILDER_DISPATCHERS_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include "utils/common/error.h"
#include "cmd/build/imagebuilder/builder.h"
#include "utils/common/regexp.h"
#include "filesys/platforms/default_unix.h"
// 全局变量
extern std::shared_ptr<Regexp> obRgex;
extern std::shared_ptr<Platform> localspec;
extern std::map<std::string, std::string> builtinBuildArgs;


void env(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs);

void from(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs);

void label(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs);

void dispatchCopy(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs);

void expose(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs);

void entrypoint(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs);

void Volume(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs);

void workdir(
    Image_Builder* b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs);

std::vector<File> processHereDocs(const std::string& originalInstruction, const std::vector<Heredoc>& heredocs, const std::vector<std::string>& args);
void checkChmodConversion(const std::string& chmod);
myerror errAtLeastOneArgument(const std::string& command);
myerror errAtLeastTwoArgument(const std::string& command);
myerror errExactlyOneArgument(const std::string& command);
myerror errTooManyArguments(const std::string& command);
myerror errNotJSON(const std::string& command);

#endif // IMAGEBUILDER_DISPATCHERS_H)
