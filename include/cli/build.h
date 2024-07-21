#if !defined(CLI_BUILD_H)
#define CLI_BUILD_H
// #include "cmd/build.h"
#include <string>
#include <vector>
#include <fstream>
#include "cobra/command.h"
#include "define/build.h"
#include "cli/common.h"
#include "define/types.h"
// #include ""
using std::string;
using std::vector;
/**
 * @brief BudResults 代表构建标志的结果
 * 
 */
struct BudResults{
    bool                allplatform=false;     ///<镜像构建是否适用所有平台
    vector<string>      annotation;  ///<向镜像元数据添加一个镜像annotation
    vector<string>      tag;         ///<镜像的标签
    vector<string>      envs;        ///<镜像的环境变量
    string              osversion;           ///<镜像支持的操作系统
    string              Authfile;
    vector<string>      BuildArg;
    vector<string>      CacheFrom;
    vector<string>      CacheTo;
    string              CacheTTL;
    string              CertDir;
    bool                Compress=false;
    vector<string>      File;
    string              Creds;
    string              CWOptions;
    bool                DisableCompression=false;
    bool                DisableContentTrust=false;
    string              Format;
    string              From;
    string              IgnoreFile; 
    string              Iidfile;
    int                 Jobs=0;
    bool                LogRusage=false;
    string              RusageLogFile;
    bool                LogSplitByPlatform=false;
    vector<string>      Label;
    vector<string>      LayerLabel;
    string              Logfile;
    string              Manifest;
    bool                NoCache=false;
    int64_t             Timestamp=0;
    bool                OmitHistory=false;
    vector<string>      OCIHooksDir;
    string              Pull;
    bool                PullAlways=false;
    bool                PullNever=false;
    bool                Quiet=false;
    bool                IdentityLabel=false;
    bool                Rm=false;
    bool                Stdin;
    string              BuildOutput;
    string              Target;
    vector<string>      OSFeatures;
    string              OSVersion;
    string              SignBy;
    string              SignaturePolicy;
    bool                SkipUnusedStages=false;
    bool                Squash=false;
    vector<string>      UnsetEnvs;
    vector<string>      UnsetLabels;

};
/**
 * @brief LayerResults 表示层标志的结果
 * 
 */
struct LayerResults{
    bool ForceRm=false; ///<
    bool Layers=false; ///<
};
/**
 * @brief FromAndBugResults 表示 build 和 from 中常见标志的结果
 * 
 */
struct FromAndBudResults{
    vector<string>      AddHost;
    string              BlobCache;
    vector<string>      CapAdd;
    vector<string>      CapDrop;
    string              CDIConfigDir;
    string              CgroupParent;
    uint64_t            CPUPeriod=0;
    int64_t             CPUQuota=0;
    string              CPUSetCPUs;
    string              CPUSetMems;
    uint64_t            CPUShares=0;
    vector<string>      DecryptionKeys;
    vector<string>      Devices;
    vector<string>      DNSSearch;
    vector<string>      DNSServers;
    vector<string>      DNSOptions;
    bool                HTTPProxy=false;
    string              Isolation;
    string              Memory;
    string              MemorySwap;
    int                 Retry=0;
    string              RetryDelay;
    vector<string>      SecurityOpt;
    string              ShmSize;
    vector<string>      Ulimit;
    vector<string>      Volumes;
};

struct UserNSResults{
    string              UserNS;
    vector<string>      GroupAdd;
    vector<string>      UserNSUIDMap;
    vector<string>      UserNSGIDMap;
    string              UserNSUIDMapUser;
    string              UserNSGIDMapGroup;
};

struct NameSpaceResults{
    string          Cgroup;
	string          IPC;
	string          Network;
	string          CNIConfigDir;
	string          CNIPlugInPath;
	string          PID;
	string          UTS;
};
/**
 * @struct BuildOptions
 * @brief BuildOptions 代表build标志的结果
 * BuildOptions 定义build命令中合法flag的声明，
 * 后续在功能扩展时，只需在BuildOptions中添加即可添加flag
 */

struct BuildOptions:public BudResults,public LayerResults,public FromAndBudResults,public UserNSResults,public NameSpaceResults{
    // BudResults*         buildFlagResults=nullptr;
    // // shared_ptr<BudResults> br;
    // LayerResults*       layerFlagsResults=nullptr;
    // FromAndBudResults*  fromAndBudResults=nullptr;
    // UserNSResults*      userNSResults=nullptr;
    std::ofstream*       logwriter=nullptr;
    // BuildOptions(BudResults*bp=nullptr,LayerResults* lp=nullptr,FromAndBudResults*fp=nullptr,UserNSResults*up=nullptr):
    //             buildFlagResults(bp),layerFlagsResults(lp),fromAndBudResults(fp),userNSResults(up){};
    // ~BuildOptions(){
    //     delete buildFlagResults;
    //     delete layerFlagsResults;
    //     delete fromAndBudResults;
    //     delete userNSResults;
    // }
    BuildOptions():BudResults(),LayerResults(),FromAndBudResults(),UserNSResults(){};
};


void GenBuildOptions(Command* cmd, vector<string> inputArgs,BuildOptions* iopts, define_BuildOptions* budopt, vector<string>& ret_containerfiles,vector<string>& removeAll);

#endif // CLI_BUILD_H
