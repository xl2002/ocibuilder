#include "utils/init/init_global.h"
#include "utils/cli/cobra/command.h"
#include "utils/logger/logrus/entry.h"
#include "utils/logger/logrus/exported.h"
#include "utils/parse/dockerfileparse.h"
#include "utils/common/regexp.h"
#include "utils/parse/line_parsers.h"
#include "utils/common/go/file.h"
#include "utils/logger/logrus/alt_exit.h"
#include "utils/parse/buildkitparser.h"
#include "utils/parse/parse.h"
// 全局变量定义
Flagset* CommandLine;

std::shared_ptr<BufferPool_interface> globalBufferPool;

std::string logrusPackage;
int minimumCallerDepth;
int maximumCallerDepth;
int knownLogrusFrames;

std::shared_ptr<Logger> Std;

std::shared_ptr<Regexp> tokenEscapeCommand;
std::shared_ptr<Regexp> tokenPlatformCommand;
std::shared_ptr<Regexp> tokenWhitespace;
std::shared_ptr<Regexp> tokenComment;
std::map<std::string, bool> heredocDirectives;
std::string defaultPlatformToken;
char DefaultEscapeToken;
std::map<std::string, bool> heredocCompoundDirectives;
namespace dockerfilecommand {
    std::string Add;
    std::string Arg;
    std::string Cmd;
    std::string Copy;
    std::string Entrypoint;
    std::string Env;
    std::string Expose;
    std::string From;
    std::string Healthcheck;
    std::string Label;
    std::string Maintainer;
    std::string Onbuild;
    std::string Run;
    std::string Shell;
    std::string StopSignal;
    std::string User;
    std::string Volume;
    std::string Workdir;
};
std::map<std::string, DispatchFunction> dispatch;

bool precompile;

char Separator;
// 定义 handlers 为一个存储函数的 vector
std::vector<std::function<void()>> handlers;

std::regex reWhitespace;
std::regex reComment;
std::regex reHeredoc;
std::regex reLeadingTabs;

// SeccompDefaultPath 定义了默认的 seccomp 配置文件路径
// std::string SeccompDefaultPath;
// SeccompOverridePath 如果存在，则覆盖默认的 seccomp 配置文件路径
// std::string SeccompOverridePath;
// TypeBind 是用于挂载主机目录的类型
std::string TypeBind;
// TypeTmpfs 是用于挂载 tmpfs 的类型
std::string TypeTmpfs;
// TypeCache 是用于挂载主机上常用的持久缓存的类型
std::string TypeCache;
// mount=type=cache 必须在主机上创建一个持久目录，以便在所有连续的构建中都可用。
// 以下目录的生命周期将继承主机如何处理临时目录的方式
std::string BuildahCacheDir;


/**
 * @brief 初始化utils模块的全局变量
 * 
 */
void init_utils(){
    CommandLine=NewFlagSet("buildah");

    globalBufferPool = std::make_shared<SimpleBufferPool>();


    logrusPackage="";
    minimumCallerDepth=1;
    maximumCallerDepth=25;
    knownLogrusFrames=4;
    Std=New();

    tokenEscapeCommand=Delayed(R"(^#[ \t]*escape[ \t]*=[ \t]*(.).*$)");
    tokenPlatformCommand=Delayed(R"(^#[ \t]*platform[ \t]*=[ \t]*(.*)$)");
    tokenWhitespace=Delayed(R"([\t\v\f\r ]+)");
    tokenComment=Delayed(R"(^#.*\t$)");
    heredocDirectives = {
        {"ADD", true},
        {"COPY", true},
        {"RUN", true}
    };
    // 默认平台标识符，假设为构建的默认平台
    defaultPlatformToken = "windows"; // 用实际的默认平台替代
    DefaultEscapeToken = '\\';
    // 定义允许包含指令的指令集合
    heredocCompoundDirectives = {
        {"ONBUILD", true}
    };
    dockerfilecommand::Add = "add";
    dockerfilecommand::Arg = "arg";
    dockerfilecommand::Cmd = "cmd";
    dockerfilecommand::Copy = "copy";
    dockerfilecommand::Entrypoint = "entrypoint";
    dockerfilecommand::Env = "env";
    dockerfilecommand::Expose = "expose";
    dockerfilecommand::From = "from";
    dockerfilecommand::Healthcheck = "healthcheck";
    dockerfilecommand::Label = "label";
    dockerfilecommand::Maintainer = "maintainer";
    dockerfilecommand::Onbuild = "onbuild";
    dockerfilecommand::Run = "run";
    dockerfilecommand::Shell = "shell";
    dockerfilecommand::StopSignal = "stopsignal";
    dockerfilecommand::User = "user";
    dockerfilecommand::Volume = "volume";
    dockerfilecommand::Workdir = "workdir";
    dispatch={
        {dockerfilecommand::From,parseStringsWhitespaceDelimited},
        {dockerfilecommand::Label,parseLabel},
        {dockerfilecommand::Copy,parseMaybeJSONToList},
        {dockerfilecommand::Env,parseEnv},
        {dockerfilecommand::Expose,parseStringsWhitespaceDelimited},
        {dockerfilecommand::Entrypoint,parseMaybeJSON},
        {dockerfilecommand::Volume,parseMaybeJSONToList}
    };
    bool precompile=false;
    // 定义路径分隔符
    #ifdef _WIN32
    Separator = '\\';  // Windows 下的路径分隔符
    #else
    Separator = '/';   // Unix-like 系统下的路径分隔符
    #endif
    
    reWhitespace.assign(R"([\t\v\f\r ]+)");
    reComment.assign(R"(^#.*$)");
    reHeredoc.assign(R"(^(\d*)<<(-?)([^<]*)$)");
    reLeadingTabs.assign(R"((^\t+))");

    // SeccompDefaultPath 定义了默认的 seccomp 配置文件路径
    // SeccompDefaultPath = "/usr/share/containers/seccomp.json";
    // SeccompOverridePath 如果存在，则覆盖默认的 seccomp 配置文件路径
    // SeccompOverridePath = "/etc/containers/seccomp.json";
    // TypeBind 是用于挂载主机目录的类型
    TypeBind = "bind";
    // TypeTmpfs 是用于挂载 tmpfs 的类型
    TypeTmpfs = "tmpfs";
    // TypeCache 是用于挂载主机上常用的持久缓存的类型
    TypeCache = "cache";
    // mount=type=cache 必须在主机上创建一个持久目录，以便在所有连续的构建中都可用。
    // 以下目录的生命周期将继承主机如何处理临时目录的方式
    BuildahCacheDir = "buildah-cache";;

}