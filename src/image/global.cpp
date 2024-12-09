#include "utils/init/init_global.h"
#include "image/buildah/buildah.h"
#include "image/types/define/pull.h"
#include "image/digest/algorithm.h"
#include "image/image_types/v1/descriptor.h"
#include "image/image_types/v1/annotations.h"
#include "image/transports/transports.h"
#include "image/image_types/docker/docker_transport.h"
#include "image/types/reference/regexp.h"
#include "image/digest/digest.h"
#include "image/shortnames/sysregistriesv2/system_registries_v2.h"
#include "image/types/define/build.h"
#include "image/types/reference/normalize.h"
#include "image/types/signature/policy_config.h"
// 全局变量定义
std::shared_ptr<PolicyTransportScopes> storageAllowedPolicyScopes;

map<string,Pull_Policy>PolicyMap;

Algorithm_sha256 SHA256;
Algorithm_sha256 Canonical_sha256;
std::map<Algorithm_sha256, std::regex> anchoredEncodedRegexps;

Descriptor DescriptorEmptyJSON;

std::shared_ptr<knownTransports> kt;

std::shared_ptr<dockerTransport> docker_Transport;

std::regex DigestRegexp;
std::regex DigestRegexpAnchored;

std::string builtinRegistriesConfPath;
std::string userRegistriesFile;
std::string userRegistriesDir;

string tarExt;
string solaris;
string windows;
string darwin;
string freebsd;

std::string legacyDefaultDomain;
std::string defaultDomain;
std::string officialRepoName;
std::string defaultTag;

std::string userPolicyFile;
std::string systemDefaultPolicyPath;

/**
 * @brief 初始化image的全局变量
 * 
 */
void init_image(){
    storageAllowedPolicyScopes=std::make_shared<PolicyTransportScopes>();

    PolicyMap ={
        {"missing",PullIfMissing},
        {"always", PullAlways},
        {"never",  PullNever},
        {"ifnewer", PullIfNewer}
    };

    SHA256=Algorithm_sha256("sha256");
    Canonical_sha256=SHA256;
    anchoredEncodedRegexps={{ SHA256, std::regex("^[a-f0-9]{64}$") }};

    DescriptorEmptyJSON = {
        MediaTypeEmptyJSON,
        "sha256:44136fa355b3678a1146ad16f7e8649e94fb4fc21fe77e8310c060f61caaff8a",
        2,
        std::vector<byte>{'{','}'}
    };
    
    v1_init();
    kt=std::make_shared<knownTransports>();
    docker_Transport=std::make_shared<dockerTransport>();
    Register(docker_Transport);
    init_regexp();
    // DigestRegexp 匹配有效的摘要类型。
    DigestRegexp.assign("[a-z0-9]+(?:[.+_-][a-z0-9]+)*:[a-zA-Z0-9=_-]+");

    // DigestRegexpAnchored 匹配有效的摘要类型，要求匹配从开始到结束。
    DigestRegexpAnchored.assign("^[a-z0-9]+(?:[.+_-][a-z0-9]+)*:[a-zA-Z0-9=_-]+$");

    builtinRegistriesConfPath="D:\\oci_images\\registries.conf";
    userRegistriesFile=".config/containers/registries.conf";
    userRegistriesDir=".config/containers/registries.conf.d";

    tarExt  = "tar";
    solaris = "solaris";
    windows = "windows";
    darwin  = "darwin";
    freebsd = "freebsd";

    legacyDefaultDomain = "index.docker.io";
	defaultDomain       = "docker.io";
	officialRepoName    = "library";
	defaultTag          = "latest";

    userPolicyFile=FromSlash("/.config/containers/policy.json");
    systemDefaultPolicyPath="/etc/containers/policy.json";
}