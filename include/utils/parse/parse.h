#if !defined(UTILS_PARSE_PARSE_H)
#define UTILS_PARSE_PARSE_H
#include <string>
#include "image/types/define/pull.h"
#include "utils/cli/cobra/command.h"
#include "image/types/define/types.h"
#include "image/types/define/build.h"
#include "image/types/define/namespace.h"
#include <array>
#include <sstream>
#include "image/util/util.h"
// #include "filesys/utils/idtools.h"
// SeccompDefaultPath 定义了默认的 seccomp 配置文件路径
// extern std::string SeccompDefaultPath;
// SeccompOverridePath 如果存在，则覆盖默认的 seccomp 配置文件路径
// extern std::string SeccompOverridePath;
// TypeBind 是用于挂载主机目录的类型
extern std::string TypeBind;
// TypeTmpfs 是用于挂载 tmpfs 的类型
extern std::string TypeTmpfs;
// TypeCache 是用于挂载主机上常用的持久缓存的类型
extern std::string TypeCache;
// mount=type=cache 必须在主机上创建一个持久目录，以便在所有连续的构建中都可用。
// 以下目录的生命周期将继承主机如何处理临时目录的方式
extern std::string BuildahCacheDir;


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
