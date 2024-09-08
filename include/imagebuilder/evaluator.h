#if !defined(IMAGEBUILDER_EVALUATOR_H)
#define IMAGEBUILDER_EVALUATOR_H
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <sstream>
#include "parse/dockerfileparse.h"
#include "bytes/buffer.h"
#include "cobra/error.h"
#include "parse/dockerfileparse.h"

std::shared_ptr<Node> ParseDockerfile(std::vector<byte> r);

extern std::map<std::string,bool> replaceEnvAllowed;


struct Step{
    std::vector<std::string> Env;
    std::vector<std::string> Args;
    std::string Command;
    std::vector<std::string> Flags;
    std::string Message;
    std::string Original;
    std::map<std::string,bool> Attrs;
    std::vector<std::shared_ptr<Heredoc>> Heredocs;
};

#endif // IMAGEBUILDER_EVALUATOR_H
