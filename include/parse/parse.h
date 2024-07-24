#if !defined(PARSE_PARSE_H)
#define PARSE_PARSE_H
#include <string>
#include "define/pull.h"
#include "cobra/command.h"
#include "define/types.h"
#include "define/build.h"
#include "define/namespace.h"
#include <array>
#include <sstream>
#include "util/util.h"
// #include "idtools/idtools.h"

PullPolicy PullPolicyFromOptions(Command* c);
shared_ptr<SystemContext> SystemContextFromOptions(Command* c);
std::string GetTempDir();
shared_ptr<Isolation> IsolationOption(string isolation);
shared_ptr<CommonBuildOptions> CommonbuildOptions(Command* cmd);
void parseSecurityOpts(vector<string> securityOpts,shared_ptr<CommonBuildOptions> commonOpts);
shared_ptr<NamespaceOptions> Namespaceoptions(Command* cmd,NetworkConfigurationPolicy& networkPolicy);
shared_ptr<NamespaceOptions> idmappingOptions(Command* cmd,shared_ptr<Isolation> isolation,shared_ptr<IDMappingOptions>idmapOptions);
std::vector<platforms> PlatformsFromOptions(Command* cmd);
void Volumes( std::vector<std::string> volumes);



#endif // PARSE_PARSE_H
