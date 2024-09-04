#include "imagebuilder/builder.h"
#include "go/string.h"
#include "cobra/error.h"
// ParseIgnore returns a tuple containing a list of excludes and an error message
// path should be a file with the .dockerignore format
// extracted from fsouza/go-dockerclient and modified to drop comments and
// empty lines.
std::vector<std::string> ParseIgnore(const std::string& path) {
    std::vector<std::string> excludes;
    std::string error_message;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        error_message = "Could not open file: " + path;
        // return excludes;

        throw myerror(error_message);
    }

    std::string line;
    while (std::getline(file, line)) {
        line = TrimSpace(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        // Remove leading slashes from the path
        if (line[0] == '/') {
            line = line.substr(1);
        }
        if (!line.empty()) {
            excludes.push_back(line);
        }
    }
    
    if (file.bad()) {
        error_message = "Error reading file: " + path;
        // return excludes;
        throw myerror(error_message);
    }

    return excludes;
}

std::shared_ptr<Image_Builder>NewBuilder(std::map<std::string,std::string>args){
    return newBuilderWithGlobalAllowedArgs(args,std::vector<std::string>());
}

std::shared_ptr<Image_Builder>newBuilderWithGlobalAllowedArgs(std::map<std::string,std::string>args,std::vector<std::string>globalallowedargs){
    auto allowed=std::map<string,bool>();
    for(auto& arg:globalallowedargs){
        allowed[arg]=true;
    }
    auto userArgs=std::map<string,string>();
    auto initialArgs=std::map<string,string>();

    for(auto& arg:args){
        userArgs[arg.first]=arg.second;
        initialArgs[arg.first]=arg.second;
    }
    auto builder=std::make_shared<Image_Builder>();
    builder->Args=initialArgs;
    builder->UserArgs=userArgs;
    builder->HeadingArgs=std::map<std::string,std::string>();
    builder->AllowedArgs=allowed;
    builder->GlobalAllowedArgs=globalallowedargs;
    return builder;
}

std::shared_ptr<Stages>NewStages(std::shared_ptr<Node>node,std::shared_ptr<Image_Builder>b){

    return nullptr;
}