#include "cmd/build/imagebuilder/evaluator.h"
#include "utils/logger/ProcessSafeLogger.h"
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
        logger->log_error(std::string(e.what()));
        throw;
    }
}

// std::map<std::string,bool> replaceEnvAllowed{
//     {"env",true},
//     {"arg",true},
//     {"volume",true},
//     {"label",true},
//     {"add",true},
//     {"workdir",true},
//     {"expose",true},
//     {"user",true},
//     {"stopsignal",true},
//     {"copy",true},
// };

// std::map<std::string,bool> allowWordExpansion{
//     {"expose",true}
// 
// };
void Step::Resolve(std::shared_ptr<Node> ast) {
    Heredocs = ast->Heredocs;
    std::string cmd = ast->Value;
    auto upperCasedCmd=toUpper(cmd);
    // std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    // 检查平台是否支持命令
    if (platformSupports(toLower(cmd))!="") {
        logger->log_error("Platform does not support command");
        throw myerror("Platform does not support command");
    }
    auto attrs=ast->Attributes;
    auto original=ast->Original;
    auto flags=ast->Flags;

    std::vector<std::string> strList;
    std::string msg = upperCasedCmd;

    if (!ast->Flags.empty()) {
        msg += " " + Join(ast->Flags, " ");
    }

    if (cmd == "ONBUILD") {
        if (ast->Children.empty()) {
            logger->log_error("ONBUILD requires at least one argument");
            throw myerror("ONBUILD requires at least one argument");
        }
        ast = ast->Children[0];
        strList.push_back(ast->Value);
        msg += " " + ast->Value;

        if (!ast->Flags.empty()) {
            msg += " " + Join(ast->Flags, " ");
        }
    }

    // 计算遍历节点的数量
    auto cursor = ast;
    size_t n = 0;
    while (cursor->Next != nullptr) {
        cursor = cursor->Next;
        ++n;
    }
    std::vector<std::string> msgList(n);

    size_t i = 0;
    auto envs=Env;
    for (auto next = ast->Next; next != nullptr; next = next->Next) {
        std::string str = next->Value;
        if (replaceEnvAllowed[cmd]) {
            std::vector<std::string> words;
            if (allowWordExpansion[cmd]) {
                try{
                    words=ProcessWords(str, envs);
                    strList.insert(strList.end(), words.begin(), words.end());
                }catch(const myerror& e){
                    logger->log_error(std::string(e.what()));
                    throw;
                }
            } else {
                try{
                    // auto str=ProcessWord(str, envs);
                    strList.push_back(ProcessWord(str, Env));
                }catch(const myerror& e){
                    logger->log_error(std::string(e.what()));
                    throw;
                }
            }
        } else {
            strList.push_back(str);
        }
        msgList[i] = next->Value;
        ++i;
    }

    msg += " " + Join(msgList, " ");

    Message = msg;
    Command = cmd;
    Args = strList;
    Original = original;
    Attrs = attrs;
    Flags = flags;
}