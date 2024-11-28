#if !defined(IMAGE_IMAGE_TYPES_V1_CONFIG_H)
#define IMAGE_IMAGE_TYPES_V1_CONFIG_H
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <ctime> // For std::time_t
#include "image/digest/digest.h"
#include "image/image_types/v1/descriptor.h"
#include "image/types/define/build.h"
#include "image/types/define/namespace.h"
#include "types/network.h"
#include "image/types/define/types.h"
#include "image/image_types/v1/config.h"
struct ImageConfig {
    // User defines the username or UID which the process in the container should run as.
    std::string user;

    // ExposedPorts a set of ports to expose from a container running this image.
    std::map<std::string, bool> exposedPorts;

    // Env is a list of environment variables to be used in a container.
    std::vector<std::string> env;

    // Entrypoint defines a list of arguments to use as the command to execute when the container starts.
    std::vector<std::string> entrypoint;

    // Cmd defines the default arguments to the entrypoint of the container.
    std::vector<std::string> cmd;

    // Volumes is a set of directories describing where the process is likely to write data specific to a container instance.
    std::map<std::string, bool> volumes;

    // WorkingDir sets the current working directory of the entrypoint process in the container.
    std::string workingDir;

    // Labels contains arbitrary metadata for the container.
    std::map<std::string, std::string> labels;

    // StopSignal contains the system call signal that will be sent to the container to exit.
    std::string stopSignal;

    // ArgsEscaped
    //
    // Deprecated: This field is present only for legacy compatibility with
    // Docker and should not be used by new image builders. It is used by Docker
    // for Windows images to indicate that the `Entrypoint` or `Cmd` or both,
    // contains only a single element array, that is a pre-escaped, and combined
    // into a single string `CommandLine`. If `true`, the value in `Entrypoint` or
    // `Cmd` should be used as-is to avoid double escaping.
    bool argsEscaped=false;
};
struct RootFS {
    // Type is the type of the rootfs.
    std::string type;

    // DiffIDs is an array of layer content hashes (DiffIDs), in order from bottom-most to top-most.
    std::vector<Digest> diffIDs;
};
struct History {
    // Created is the combined date and time at which the layer was created, formatted as defined by RFC 3339, section 5.6.
    std::shared_ptr<std::chrono::system_clock::time_point> created=std::make_shared<std::chrono::system_clock::time_point>();  // Using std::time_t for time representation, pointer indicates it can be null

    // CreatedBy is the command which created the layer.
    std::string createdBy;

    // Author is the author of the build point.
    std::string author;

    // Comment is a custom message set when creating the layer.
    std::string comment;

    // EmptyLayer is used to mark if the history item created a filesystem diff.
    bool emptyLayer=false;
};
struct Image {
    std::shared_ptr<std::chrono::system_clock::time_point> created=std::make_shared<std::chrono::system_clock::time_point>(); // Using std::time_t as a placeholder for date and time
    std::string author;
    std::shared_ptr<Platform> platform=std::make_shared<Platform>();
    std::shared_ptr<ImageConfig> config=std::make_shared<ImageConfig>();
    std::shared_ptr<RootFS> rootFS=std::make_shared<RootFS>();
    std::vector<History> history;
};
#endif // V1_CONFIG_H)
