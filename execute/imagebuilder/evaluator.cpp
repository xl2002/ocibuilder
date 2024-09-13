#include "imagebuilder/evaluator.h"
// #include ""
// ParseDockerfile 将提供的流解析为规范的 Dockerfile

std::shared_ptr<Node> ParseDockerfile(std::vector<byte> r) {
    auto read=std::stringstream(std::string(r.begin(),r.end()));
    try
    {
        auto result=Parse(read);
        return result->AST;
    }
    catch(const myerror& e)
    {
        throw;
    }
}

std::map<std::string,bool> replaceEnvAllowed{
    {"env",true},
    {"arg",true},
    {"volume",true},
    {"label",true},
    {"add",true},
    {"workdir",true},
    {"expose",true},
    {"user",true},
    {"stopsignal",true},
    {"copy",true},
};

std::map<std::string,bool> allowWordExpansion{
    {"expose",true}

};
