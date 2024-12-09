#if !defined(IMAGE_TYPES_DEFINE_TYPES_H)
#define IMAGE_TYPES_DEFINE_TYPES_H
#include <string>
#include "image/types/reference/reference.h"
#include "image/types/internal/types.h"
#include "filesys/system/specs/specs.h"
#include "filesys/utils/idtools.h"
#include "image/types/define/pull.h"
#include "image/types/internal/types.h"
#include "image/types/types.h"
using std::string;
typedef string TeeType;

const std::string
	BuilderIdentityAnnotation = "io.buildah.version",
    Package="buildah",
    version="1.0",
    DefaultRuntime="runc",
    OCIv1ImageManifest="application/vnd.oci.image.manifest.v1+json",
    Dockerv2ImageManifest="application/vnd.docker.distribution.manifest.v2+json",
    OCI="oci",
    DOCKER="docker",
	SEV="sev",
	SNP="snp";
struct IDMap;
void TempDirForURL(std::string dir,std::string prefix, std::string url,std::string& name, std::string& subdir);
enum ShortNameMode {
    ShortNameModeInvalid = 0,
    ShortNameModeDisabled,
    ShortNameModePermissive,
    ShortNameModeEnforcing
};
enum class OptionalBool : unsigned char {
    OptionalBoolUndefined = 0,
    OptionalBoolTrue,
    OptionalBoolFalse
};
OptionalBool NewOptionalBool(bool b);
struct DockerAuthConfig  {
	string Username ;
	string Password ;
	// IdentityToken can be used as an refresh_token in place of username and
	// password to obtain the bearer/access token in oauth2 flow. If identity
	// token is set, password should not be set.
	// Ref: https://docs.docker.com/registry/spec/auth/oauth/
	string IdentityToken ;
};
struct Algorithm;
struct SystemContext  {
	string RootForImplicitAbsolutePaths; 

	// === Global configuration overrides ===
	// If not "", overrides the system's default path for signature.Policy configuration.
	string SignaturePolicyPath;
	// If not "", overrides the system's default path for registries.d (Docker signature storage configuration)
	string RegistriesDirPath;
	// Path to the system-wide registries configuration file
	string SystemRegistriesConfPath;
	// Path to the system-wide registries configuration directory
	string SystemRegistriesConfDirPath;
	// Path to the user-specific short-names configuration file
	string UserShortNameAliasConfPath;
	// If set, short-name resolution in shortnames must follow the specified mode
	std::shared_ptr<ShortNameMode> shortNameMode=std::make_shared<ShortNameMode>();
	// If set, short names will resolve in shortnames to docker.io only, and unqualified-search registries and
	// short-name aliases in registries.conf are ignored.  Note that this field is only intended to help enforce
	// resolving to Docker Hub in the Docker-compatible REST API of Podman; it should never be used outside this
	// specific context.
	bool PodmanOnlyShortNamesIgnoreRegistriesConfAndForceDockerHub=false;
	// If not "", overrides the default path for the registry authentication file, but only new format files
	string AuthFilePath;
	// if not "", overrides the default path for the registry authentication file, but with the legacy format;
	// the code currently will by default look for legacy format files like .dockercfg in the $HOME dir;
	// but in addition to the home dir, openshift may mount .dockercfg files (via secret mount)
	// in locations other than the home dir; openshift components should then set this field in those cases;
	// this field is ignored if `AuthFilePath` is set (we favor the newer format);
	// only reading of this data is supported;
	string LegacyFormatAuthFilePath;
	// If set, a path to a Docker-compatible "config.json" file containing credentials; and no other files are processed.
	// This must not be set if AuthFilePath is set.
	// Only credentials and credential helpers in this file apre processed, not any other configuration in this file.
	string DockerCompatAuthFilePath;
	// If not "", overrides the use of platform.GOARCH when choosing an image or verifying architecture match.
	string ArchitectureChoice;
	// If not "", overrides the use of platform.GOOS when choosing an image or verifying OS match.
	string OSChoice ;
	// If not "", overrides the use of detected ARM platform variant when choosing an image or verifying variant match.
	string VariantChoice;
	// If not "", overrides the system's default directory containing a blob info cache.
	string BlobInfoCacheDir;
	// Additional tags when creating or copying a docker-archive.
	vector<std::shared_ptr<NamedTagged_interface>> DockerArchiveAdditionalTags;
	// If not "", overrides the temporary directory to use for storing big files
	string BigFilesTemporaryDir;

	// === OCI.Transport overrides ===
	// If not "", a directory containing a CA certificate (ending with ".crt"),
	// a client certificate (ending with ".cert") and a client certificate key
	// (ending with ".key") used when downloading OCI image layers.
	string OCICertPath;
	// Allow downloading OCI image layers over HTTP, or HTTPS with failed TLS verification. Note that this does not affect other TLS connections.
	bool OCIInsecureSkipTLSVerify=false;
	// If not "", use a shared directory for storing blobs rather than within OCI layouts
	string OCISharedBlobDirPath;
	// Allow UnCompress image layer for OCI image layer
	bool OCIAcceptUncompressedLayers=false;

	// === docker.Transport overrides ===
	// If not "", a directory containing a CA certificate (ending with ".crt"),
	// a client certificate (ending with ".cert") and a client certificate key
	// (ending with ".key") used when talking to a container registry.
	string DockerCertPath;
	// If not "", overrides the system’s default path for a directory containing host[:port] subdirectories with the same structure as DockerCertPath above.
	// Ignored if DockerCertPath is non-empty.
	string DockerPerHostCertDirPath;
	// Allow contacting container registries over HTTP, or HTTPS with failed TLS verification. Note that this does not affect other TLS connections.
	OptionalBool DockerInsecureSkipTLSVerify ;
	// if nil, the library tries to parse ~/.docker/config.json to retrieve credentials
	// Ignored if DockerBearerRegistryToken is non-empty.
	std::shared_ptr<DockerAuthConfig> dockerAuthConfig = std::make_shared<DockerAuthConfig>();
	// if not "", the library uses this registry token to authenticate to the registry
	string DockerBearerRegistryToken;
	// if not "", an User-Agent header is added to each request when contacting a registry.
	string DockerRegistryUserAgent;
	// if true, a V1 ping attempt isn't done to give users a better error. Default is false.
	// Note that this field is used mainly to integrate containers/image into projectatomic/docker
	// in order to not break any existing docker's integration tests.
	bool DockerDisableV1Ping=false;
	// If true, dockerImageDestination.SupportedManifestMIMETypes will omit the Schema1 media types from the supported list
	bool DockerDisableDestSchema1MIMETypes=false;
	// If true, the physical pull source of docker transport images logged as info level
	bool DockerLogMirrorChoice=false;
	// Directory to use for OSTree temporary files
	string OSTreeTmpDirPath;
	// If true, all blobs will have precomputed digests to ensure layers are not uploaded that already exist on the registry.
	// Note that this requires writing blobs to temporary files, and takes more time than the default behavior,
	// when the digest for a blob is unknown.
	bool DockerRegistryPushPrecomputeDigests=false;

	// === docker/daemon.Transport overrides ===
	// A directory containing a CA certificate (ending with ".crt"),
	// a client certificate (ending with ".cert") and a client certificate key
	// (ending with ".key") used when talking to a Docker daemon.
	string DockerDaemonCertPath;
	// The hostname or IP to the Docker daemon. If not set (aka ""), client.DefaultDockerHost is assumed.
	string DockerDaemonHost;
	// Used to skip TLS verification, off by default. To take effect DockerDaemonCertPath needs to be specified as well.
	bool DockerDaemonInsecureSkipTLSVerify=false;

	// === dir.Transport overrides ===
	// DirForceCompress compresses the image layers if set to true
	bool DirForceCompress=false;
	// DirForceDecompress decompresses the image layers if set to true
	bool DirForceDecompress=false;

	// CompressionFormat is the format to use for the compression of the blobs
	std::shared_ptr<Algorithm> CompressionFormat=std::make_shared<Algorithm>();
	// CompressionLevel specifies what compression level is used
	std::shared_ptr<int> CompressionLevel=std::make_shared<int>(0);
	SystemContext()= default;
	~SystemContext()= default;
};

struct AutoUserNsOptions{
    uint32_t Size=0;
    uint32_t InitialSize=0;
    std::string PasswdFile;
    std::string GroupFile;
    std::vector<IDMap> AdditionalUIDMappings;
    std::vector<IDMap> AdditionalGIDMappings;
	AutoUserNsOptions()= default;
};
struct IDMappingOptions{
	bool HostUIDMapping=false;
	bool HostGIDMapping=false;
	vector<LinuxIDMapping> UIDMap;
	vector<LinuxIDMapping> GIDMap;
	bool AutoUserNs=false;
	std::shared_ptr<AutoUserNsOptions> AutoUserNsOpts=std::make_shared<AutoUserNsOptions>();
	IDMappingOptions()= default;
};
struct Secret{
	std::string ID;
	std::string Source;
	std::string SourceType;
};

struct BuildOutputOption{
	std::string Path;
	bool IsDir=false;
	bool IsStdout=false;
};
struct ConfidentialWorkloadOptions{
	bool Convert=false;
	std::string AttestationURL;
	int CPUs=0;
	int Memory=0;
	std::string TempDir;
	std::shared_ptr<TeeType> teeType=nullptr;
	bool IgnoreAttestationErrors=false;
	std::string WorkloadID;
	std::string DiskEncryptionPassphrase;
	std::string Slop;
	std::string FirmwareLibrary;
	ConfidentialWorkloadOptions()= default;
};
typedef std::string SBOMMergeStrategy;
struct SBOMScanOptions {
    std::vector<std::string> Type;    // 一个缩短的名称，表示已定义的这些选项的组
    std::string Image;                // 要使用的扫描器映像
    PullPolicy PullPolicy;           // 获取扫描器映像的方式
    std::vector<std::string> Commands; // 要用于图像根文件系统或 ContextDir 位置的一个或多个命令
    std::vector<std::string> ContextDir; // 要查找的目录位置
    std::string SBOMOutput;           // 在外部存储的 SBOM 扫描器输出（即本地文件系统）
    std::string PURLOutput;           // 在外部存储的 PURL 列表（即本地文件系统）
    std::string ImageSBOMOutput;      // 在图像中存储的 SBOM 扫描器输出
    std::string ImagePURLOutput;      // 在图像中存储的 PURL 列表
    SBOMMergeStrategy MergeStrategy;  // 多次扫描的输出合并方式
};
struct LayerCompression;
struct BlobInfo{
	std::shared_ptr<Digest> Digest=std::make_shared<::Digest>();
	int64_t Size=0;
	std::vector<std::string> URLs;
	std::map<std::string, std::string> Annotations;
	std::string MediaType;
	std::shared_ptr<LayerCompression> CompressionOperation=std::make_shared<LayerCompression>();
	std::shared_ptr<Algorithm> CompressionAlgorithm=std::make_shared<Algorithm>();
	BlobInfo()=default;
};
enum class progressevent:uint8_t{
	ProgressEventNewArtifact,
	ProgressEventRead,
	ProgressEventDone,
	ProgressEventSkipped
};
struct ProgressEvent{
	progressevent value;
};

struct ProgressProperties{
	std::shared_ptr<ProgressProperties> Event=std::make_shared<ProgressProperties>();
	std::shared_ptr<BlobInfo> Artifact=std::make_shared<BlobInfo>();
	uint64_t Offset=0;
	uint64_t OffsetUpdate=0;
};

#endif // DEFINE_TYPES_H
