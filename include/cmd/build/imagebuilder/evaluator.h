#if !defined(CMD_BUILD_IMAGEBUILDER_EVALUATOR_H)
#define CMD_BUILD_IMAGEBUILDER_EVALUATOR_H
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <sstream>
#include "utils/parse/dockerfileparse.h"
#include "utils/common/buffer.h"
#include "utils/common/error.h"
// #include "utils/parse/dockerfileparse.h"
#include "utils/common/go/string.h"
#include "cmd/build/imagebuilder/internals.h"
#include "cmd/build/imagebuilder/shell_parser.h"
std::shared_ptr<Node> ParseDockerfile(std::vector<byte> r);

extern std::map<std::string,bool> replaceEnvAllowed;
extern std::map<std::string,bool> allowWordExpansion;

class Step{
    public:
    std::vector<std::string> Env;
    std::vector<std::string> Args;
    std::string Command;
    std::vector<std::string> Flags;
    std::string Message;
    std::string Original;
    std::map<std::string,bool> Attrs;
    std::vector<Heredoc> Heredocs;
    Step()=default;
    void Resolve(std::shared_ptr<Node> ast);
};



#endif // IMAGEBUILDER_EVALUATOR_H
