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

void env(
    std::shared_ptr<Image_Builder>b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs);

void from(
    std::shared_ptr<Image_Builder>b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs);

void label(
    std::shared_ptr<Image_Builder>b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs);

void dispatchCopy(
    std::shared_ptr<Image_Builder>b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs);

void expose(
    std::shared_ptr<Image_Builder>b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs);

void entrypoint(
    std::shared_ptr<Image_Builder>b,
    std::vector<std::string>args,
    std::map<std::string,bool>attributes,
    std::vector<std::string>flagArgs,
    std::string original,
    std::vector<Heredoc>heredocs);

void Volume(
    std::shared_ptr<Image_Builder>b,
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
