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
#include "network/network.h"
#include "image/types/define/types.h"
#include "image/image_types/v1/config.h"
#include "boost/json.hpp"
#include "utils/common/go/string.h"
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
    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const ImageConfig& image) {
        jv=boost::json::object{
            // {"user",image.user},
            // {"exposedPorts",boost::json::value_from(image.exposedPorts)},
            {"env",boost::json::value_from(image.env)},
            // {"entrypoint",boost::json::value_from(image.entrypoint)},
            {"cmd",boost::json::value_from(image.cmd)},
            // {"volumes",boost::json::value_from(image.volumes)},
            // {"workingDir",image.workingDir},
            {"labels",boost::json::value_from(image.labels)}
            // {"stopSignal",image.stopSignal},
            // {"argsEscaped",image.argsEscaped}
        };
    }
    friend ImageConfig tag_invoke(boost::json::value_to_tag<ImageConfig>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        ImageConfig image;
        // image.user=obj.at("user").as_string();
        // image.exposedPorts=boost::json::value_to<std::map<std::string, bool>>(obj.at("exposedPorts"));
        image.env=boost::json::value_to<std::vector<std::string>>(obj.at("env"));
        // image.entrypoint=boost::json::value_to<std::vector<std::string>>(obj.at("entrypoint"));
        image.cmd=boost::json::value_to<std::vector<std::string>>(obj.at("cmd"));
        // image.volumes=boost::json::value_to<std::map<std::string, bool>>(obj.at("volumes"));
        // image.workingDir=obj.at("workingDir").as_string();
        image.labels=boost::json::value_to<std::map<std::string, std::string>>(obj.at("labels"));
        // image.stopSignal=obj.at("stopSignal").as_string();
        // image.argsEscaped=obj.at("argsEscaped").as_bool();
        return image;
    }

};
struct RootFS {
    // Type is the type of the rootfs.
    std::string type;

    // DiffIDs is an array of layer content hashes (DiffIDs), in order from bottom-most to top-most.
    std::vector<std::string> diffIDs;
    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const RootFS& image) {
        jv=boost::json::object{
            {"type",image.type},
            {"diff_ids",boost::json::value_from(image.diffIDs)}
        };
    }
    friend RootFS tag_invoke(boost::json::value_to_tag<RootFS>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        RootFS image;
        image.type=obj.at("type").as_string().c_str();
        image.diffIDs=boost::json::value_to<std::vector<std::string>>(obj.at("diff_ids"));
        return image;
    }
};
struct History {
    // Created is the combined date and time at which the layer was created, formatted as defined by RFC 3339, section 5.6.
    std::chrono::system_clock::time_point created;  // Using std::time_t for time representation, pointer indicates it can be null

    // CreatedBy is the command which created the layer.
    std::string createdBy;

    // Author is the author of the build point.
    std::string author;

    // Comment is a custom message set when creating the layer.
    std::string comment;

    // EmptyLayer is used to mark if the history item created a filesystem diff.
    bool emptyLayer=false;
    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const History& image) {
        jv=boost::json::object{
            {"created",timePointToISOString(image.created)},
            {"created_by",image.createdBy},
            {"author",image.author},
            {"comment",image.comment},
            {"empty_layer",image.emptyLayer}
        };
    }
    friend History tag_invoke(boost::json::value_to_tag<History>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        History image;
        image.created=parseISOStringToTimePoint(obj.at("created").as_string().c_str());
        image.createdBy=obj.at("created_by").as_string().c_str();
        image.author=obj.at("author").as_string().c_str();
        image.comment=obj.at("comment").as_string().c_str();
        image.emptyLayer=obj.at("empty_layer").as_bool();
        return image;
    }
};
namespace v1 {
    struct Image {
        std::chrono::system_clock::time_point created; // Using std::time_t as a placeholder for date and time
        std::string author;
        Platform platform;
        ImageConfig config;
        RootFS rootFS;
        std::vector<History> history;
        Image()=default;
        /**
         * @brief 序列化
         * 
         * @param jv 
         * @param image 
         */
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const Image& image) {
            jv=boost::json::object{
                {"created",timePointToISOString(image.created)},
                // {"author",image.author},
                {"architecture",boost::json::value_from(image.platform.Architecture)},
                {"os",boost::json::value_from(image.platform.OS)},
                {"config",boost::json::value_from(image.config)},
                {"rootfs",boost::json::value_from(image.rootFS)},
                {"history",boost::json::value_from(image.history)}
            };
        }
        /**
         * @brief 反序列化
         * 
         * @param jv 
         * @param image 
         */
        friend Image tag_invoke(boost::json::value_to_tag<Image>, const boost::json::value& jv) {
            auto obj=jv.as_object();
            Image image;
            image.created=parseISOStringToTimePoint(obj.at("created").as_string().c_str());
            // image.author=obj.at("author").as_string().c_str();
            image.platform.Architecture=obj.at("architecture").as_string().c_str();
            image.platform.OS=obj.at("os").as_string().c_str();
            image.config=boost::json::value_to<ImageConfig>(obj.at("config"));
            image.rootFS=boost::json::value_to<RootFS>(obj.at("rootfs"));
            image.history=boost::json::value_to<std::vector<History>>(obj.at("history"));
            return image;
        }
    };
}

struct Manifest{
    int SchemaVersion=0;
    std::string MediaType;
    std::string ArtifactType;
    Descriptor Config;
    std::vector<Descriptor> Layers;
    std::shared_ptr<Descriptor> Subject=std::make_shared<Descriptor>();
    std::map<std::string, std::string> Annotations;
    Manifest()=default;
    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const Manifest& image) {
        jv=boost::json::object{
            {"schemaVersion",image.SchemaVersion},
            {"config",boost::json::value_from(image.Config)},
            {"layers",boost::json::value_from(image.Layers)},
            {"annotations", boost::json::value_from(image.Annotations)}
        };
    }

    friend Manifest tag_invoke(boost::json::value_to_tag<Manifest>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        Manifest m;
        m.SchemaVersion=obj.at("schemaVersion").as_int64();
        m.Config=boost::json::value_to<Descriptor>(obj.at("config"));
        m.Layers=boost::json::value_to<std::vector<Descriptor>>(obj.at("layers"));
        m.Annotations=boost::json::value_to<std::map<std::string, std::string>>(obj.at("annotations"));
        return m;
    };
};
#endif // V1_CONFIG_H)
