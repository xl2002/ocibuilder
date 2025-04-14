#include "utils/init/init_global.h"
#include "image/buildah/buildah.h"
#include "image/types/define/pull.h"
#include "image/digest/algorithm.h"
#include "image/image_types/v1/descriptor.h"
#include "image/image_types/v1/annotations.h"
#include "image/image_types/v1/mediatype.h"
#include "image/transports/transports.h"
#include "image/image_types/docker/docker_transport.h"
#include "image/types/reference/regexp.h"
#include "image/digest/digest.h"
#include "image/shortnames/sysregistriesv2/system_registries_v2.h"
#include "image/types/define/build.h"
#include "image/types/reference/normalize.h"
#include "image/types/signature/policy_config.h"
#include "image/types/define/types.h"
// 全局变量定义
/// @brief 存储允许的策略作用域
/// @details 定义不同传输协议允许的策略作用域
std::shared_ptr<PolicyTransportScopes> storageAllowedPolicyScopes;

/// @brief 拉取策略映射表
/// @details 将字符串策略名称映射到对应的Pull_Policy枚举值
/// 包含以下策略:
/// - "missing": PullIfMissing
/// - "always": PullAlways  
/// - "never": PullNever
/// - "ifnewer": PullIfNewer
map<string,Pull_Policy>PolicyMap;

Algorithm_sha256 sha_256;
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

/// @brief 内容描述符的媒体类型
std::string MediaTypeDescriptor;
/// @brief oci-layout的媒体类型
std::string MediaTypeLayoutHeader;
/// @brief 镜像索引的媒体类型  
std::string MediaTypeImageIndex;
/// @brief 镜像清单的媒体类型
std::string MediaTypeImageManifest;
/// @brief 镜像配置的媒体类型
std::string MediaTypeImageConfig;
/// @brief 空JSON对象的媒体类型
std::string MediaTypeEmptyJSON;
/// @brief 镜像层的媒体类型(tar格式)
std::string MediaTypeImageLayer;
/// @brief 镜像层的媒体类型(gzip压缩)
std::string MediaTypeImageLayerGzip;
/// @brief 镜像层的媒体类型(zstd压缩) 
std::string MediaTypeImageLayerZstd;
std::string BuildAuthor;

string tarExt;
string solaris;
string windows;
string darwin;
string freebsd;

/// @brief 旧版默认域名(index.docker.io)
std::string legacyDefaultDomain;
/// @brief 当前默认域名(docker.io)
std::string defaultDomain;
/// @brief 官方仓库名称(library)
std::string officialRepoName;
/// @brief 默认标签(latest)
std::string defaultTag;

std::string userPolicyFile;
std::string systemDefaultPolicyPath;

std::string BuilderIdentityAnnotation;
std::string Package;
std::string version;
std::string DefaultRuntime;
std::string OCIv1ImageManifest;
std::string Dockerv2ImageManifest;
std::string DockerV2Schema2ConfigMediaType;
std::string DockerV2Schema2LayerMediaType;
std::string OCI;
std::string DOCKER;
std::string SEV;
std::string SNP;
/**
 * @brief 初始化镜像相关的全局变量和配置
 * 
 * @details 该函数负责初始化整个镜像模块所需的全局变量，包括：
 * 1. 拉取策略映射(PolicyMap)
 * 2. 摘要算法配置(sha_256)
 * 3. 媒体类型定义(MediaType*)
 * 4. 默认域名和标签配置(defaultDomain/defaultTag)
 * 5. 传输协议注册(Register)
 * 6. 正则表达式模式(DigestRegexp)
 * 
 * @note 该函数应在程序启动时调用一次，确保所有全局变量正确初始化
 */
void init_image(){
    storageAllowedPolicyScopes=std::make_shared<PolicyTransportScopes>();

    PolicyMap ={
        {"missing",PullIfMissing},
        {"always", PullAlways},
        {"never",  PullNever},
        {"ifnewer", PullIfNewer}
    };

    sha_256=Algorithm_sha256("sha256");
    Canonical_sha256=sha_256;
    anchoredEncodedRegexps={{ sha_256, std::regex("^[a-f0-9]{64}$") }};

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

    builtinRegistriesConfPath=".\\oci_images\\registries.conf";
    userRegistriesFile=".config/containers/registries.conf";
    userRegistriesDir=".config/containers/registries.conf.d";

    // MediaTypeDescriptor specifies the media type for a content descriptor.
	MediaTypeDescriptor = "application/vnd.oci.descriptor.v1+json";
	// MediaTypeLayoutHeader specifies the media type for the oci-layout.
	MediaTypeLayoutHeader = "application/vnd.oci.layout.header.v1+json";
	// MediaTypeImageIndex specifies the media type for an image index.
	MediaTypeImageIndex = "application/vnd.oci.image.index.v1+json";
	// MediaTypeImageManifest specifies the media type for an image manifest.
	// MediaTypeImageManifest = "application/vnd.oci.image.manifest.v1+json";
    MediaTypeImageManifest = "application/vnd.docker.distribution.manifest.v2+json";
	// MediaTypeImageConfig specifies the media type for the image configuration.
	// MediaTypeImageConfig = "application/vnd.oci.image.config.v1+json";
    MediaTypeImageConfig = "application/vnd.docker.container.image.v1+json";
	// MediaTypeEmptyJSON specifies the media type for an unused blob containing the value "{}".
	MediaTypeEmptyJSON = "application/vnd.oci.empty.v1+json";
        // MediaTypeImageLayer is the media type used for layers referenced by the manifest.
	// MediaTypeImageLayer = "application/vnd.oci.image.layer.v1.tar";
    MediaTypeImageLayer = "application/vnd.docker.image.rootfs.diff.tar";
	// MediaTypeImageLayerGzip is the media type used for gzipped layers
	// referenced by the manifest.
	// MediaTypeImageLayerGzip = "application/vnd.oci.image.layer.v1.tar+gzip";
    MediaTypeImageLayerGzip = "application/vnd.docker.image.rootfs.diff.tar.gzip";
	// MediaTypeImageLayerZstd is the media type used for zstd compressed
	// layers referenced by the manifest.
	MediaTypeImageLayerZstd = "application/vnd.oci.image.layer.v1.tar+zstd";
    BuildAuthor="Northwestern Polytechnical University";
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

	BuilderIdentityAnnotation = "io.buildah.version";
    Package="buildah";
    version="1.0";
    DefaultRuntime="runc";
    OCIv1ImageManifest="application/vnd.oci.image.manifest.v1+json";
    Dockerv2ImageManifest="application/vnd.docker.distribution.manifest.v2+json";
    DockerV2Schema2ConfigMediaType = "application/vnd.docker.container.image.v1+json";
    DockerV2Schema2LayerMediaType = "application/vnd.docker.image.rootfs.diff.tar.gzip";
    OCI="oci";
    DOCKER="docker";
	SEV="sev";
	SNP="snp";
}
