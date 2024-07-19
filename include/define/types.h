#if !defined(DEFINE_TYPES_H)
#define DEFINE_TYPES_H
#include <string>
#include "docker/reference.h"
#include "pkg/internal/types.h"
#include "define/specs.h"
#include "pkg/idtools/idtools.h"
using std::string;
const std::string
    Package="buildah",
    Version="1.0",
    DefaultRuntime="runc",
    OCIv1ImageManifest="application/vnd.oci.image.manifest.v1+json",
    Dockerv2ImageManifest="application/vnd.docker.distribution.manifest.v2+json",
    OCI="oci",
    DOCKER="docker";

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
	// If set, short-name resolution in pkg/shortnames must follow the specified mode
	ShortNameMode* ShortNameMode=nullptr;
	// If set, short names will resolve in pkg/shortnames to docker.io only, and unqualified-search registries and
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
	vector<NamedTagged> DockerArchiveAdditionalTags;
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
	DockerAuthConfig* DockerAuthConfig =nullptr;
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
	Algorithm* CompressionFormat=nullptr;
	// CompressionLevel specifies what compression level is used
	int *CompressionLevel=nullptr;
};

struct AutoUserNsOptions{
    uint32_t Size;
    uint32_t InitialSize;
    std::string PasswdFile;
    std::string GroupFile;
    std::vector<IDMap> AdditionalUIDMappings;
    std::vector<IDMap> AdditionalGIDMappings;
};
struct IDMappingOptions{
	bool HostUIDMapping;
	bool HostGIDMapping;
	vector<LinuxIDMapping> UIDMap;
	vector<LinuxIDMapping> GIDMap;
	bool AutoUserNs;
	AutoUserNsOptions AutoUserNsOpts;
};



#endif // DEFINE_TYPES_H
