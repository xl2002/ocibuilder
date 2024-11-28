#if !defined(PARSE_PARSE_H)
#define PARSE_PARSE_H
#include <string>
#include "image/types/define/pull.h"
#include "cobra/command.h"
#include "image/types/define/types.h"
#include "image/types/define/build.h"
#include "image/types/define/namespace.h"
#include <array>
#include <sstream>
#include "image/util/util.h"
// #include "filesys/utils/idtools.h"

PullPolicy PullPolicyFromOptions(Command* c);
shared_ptr<SystemContext> SystemContextFromOptions(Command* c);
std::string GetTempDir();
shared_ptr<Isolation> IsolationOption(string isolation);
shared_ptr<CommonBuildOptions> CommonbuildOptions(Command* cmd);
void parseSecurityOpts(vector<string> securityOpts,shared_ptr<CommonBuildOptions> commonOpts);
shared_ptr<NamespaceOptions> Namespaceoptions(Command* cmd,std::shared_ptr<NetworkConfigurationPolicy> networkPolicy);
shared_ptr<NamespaceOptions> idmappingOptions(Command* cmd,shared_ptr<Isolation> isolation,shared_ptr<IDMappingOptions>idmapOptions);
std::vector<platforms> PlatformsFromOptions(Command* cmd);
void Volumes( std::vector<std::string> volumes);
std::tuple<std::vector<std::string>, std::string> ContainerIgnoreFile(
    const std::string& contextDir, const std::string& path, const std::vector<std::string>& containerFiles);
std::map<std::string,std::shared_ptr<Secret>> Secrets(std::vector<std::string> secrets);
#endif // PARSE_PARSE_H
