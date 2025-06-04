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
class ImageConfig {
    public:
    // User defines the username or UID which the process in the container should run as.
    std::string user;
    // ExposedPorts a set of ports to expose from a container running this image.
    std::map<std::string, boost::json::object> exposedPorts;
    // Env is a list of environment variables to be used in a container.
    std::vector<std::string> env;
    // Entrypoint defines a list of arguments to use as the command to execute when the container starts.
    std::vector<std::string> entrypoint;
    // Cmd defines the default arguments to the entrypoint of the container.
    std::vector<std::string> cmd;
    // Volumes is a set of directories describing where the process is likely to write data specific to a container instance.
    // std::map<std::string, std::string> volumes;
    std::map<std::string, boost::json::object> volumes;
    // WorkingDir sets the current working directory of the entrypoint process in the container.
    std::string workingDir;
    // Labels contains arbitrary metadata for the container.
    std::map<std::string, std::string> labels;
    // StopSignal contains the system call signal that will be sent to the container to exit.
    std::string stopSignal;
    // Image is the name of the image used to create the container.
    std::string Image;
    // OnBuild is a list of instructions that should be executed when the image is used as a base for another build.
    std::vector<std::string> OnBuild;
    // Shell is a list of commands to run when the container starts.
    std::vector<std::string> Shell;
    // ArgsEscaped indicates whether or not the command arguments are shell-escaped.
    bool argsEscaped=false;

    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const ImageConfig& image) {
        jv=boost::json::object{
            {"User",image.user},
            {"Env",boost::json::value_from(image.env)},
            {"Cmd",boost::json::value_from(image.cmd)},
            // {"Image",image.Image},
            {"Volumes",boost::json::value_from(image.volumes)},
            {"WorkingDir",image.workingDir},
            {"Entrypoint",boost::json::value_from(image.entrypoint)},
            {"ExposedPorts",boost::json::value_from(image.exposedPorts)},
            {"Labels",boost::json::value_from(image.labels)},
            {"StopSignal",image.stopSignal},
            // {"OnBuild",boost::json::value_from(image.OnBuild)},
            {"Shell",boost::json::value_from(image.Shell)},
            // {"ArgsEscaped",image.argsEscaped}
        };
    }
    friend ImageConfig tag_invoke(boost::json::value_to_tag<ImageConfig>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        ImageConfig image;
        image.user=obj.at("User").as_string().c_str();
        image.env=boost::json::value_to<std::vector<std::string>>(obj.at("Env"));
        image.cmd=boost::json::value_to<std::vector<std::string>>(obj.at("Cmd"));
        // image.Image=obj.at("Image").as_string().c_str();
        image.volumes=boost::json::value_to<std::map<std::string, boost::json::object>>(obj.at("Volumes"));
        image.workingDir=obj.at("WorkingDir").as_string().c_str();
        image.entrypoint=boost::json::value_to<std::vector<std::string>>(obj.at("Entrypoint"));
        image.exposedPorts=boost::json::value_to<std::map<std::string, boost::json::object>>(obj.at("ExposedPorts"));
        image.labels=boost::json::value_to<std::map<std::string, std::string>>(obj.at("Labels"));
        image.stopSignal=obj.at("StopSignal").as_string().c_str();
        // image.OnBuild=boost::json::value_to<std::vector<std::string>>(obj.at("OnBuild"));
        image.Shell=boost::json::value_to<std::vector<std::string>>(obj.at("Shell"));
        // image.argsEscaped=obj.at("ArgsEscaped").as_bool();
        return image;
    }

};
class RootFS {
    public:
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
class History {
    public:
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
    class Image {
        public:
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
                {"author",image.author},
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
            image.author=obj.at("author").as_string().c_str();
            image.platform.Architecture=obj.at("architecture").as_string().c_str();
            image.platform.OS=obj.at("os").as_string().c_str();
            image.config=boost::json::value_to<ImageConfig>(obj.at("config"));
            image.rootFS=boost::json::value_to<RootFS>(obj.at("rootfs"));
            image.history=boost::json::value_to<std::vector<History>>(obj.at("history"));
            return image;
        }
    };
}

class Manifest{
    public:
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
            {"mediaType",image.MediaType},
            {"config",boost::json::value_from(image.Config)},
            {"layers",boost::json::value_from(image.Layers)}
            // {"annotations", boost::json::value_from(image.Annotations)}
        };
    }


    friend Manifest tag_invoke(boost::json::value_to_tag<Manifest>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        Manifest m;
        m.SchemaVersion=obj.at("schemaVersion").as_int64();
        m.MediaType=obj.at("mediaType").as_string().c_str();
        m.Config=boost::json::value_to<Descriptor>(obj.at("config"));
        m.Layers=boost::json::value_to<std::vector<Descriptor>>(obj.at("layers"));
        // m.Annotations=boost::json::value_to<std::map<std::string, std::string>>(obj.at("annotations"));
        return m;
    };
};
class Pathlist{
    public:
    std::string path;
    std::string value;
    Pathlist()=default;

    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const Pathlist& image) {
        jv=boost::json::object{
            {"path",image.path},
            {"value",image.value}
        };
    }
    friend Pathlist tag_invoke(boost::json::value_to_tag<Pathlist>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        Pathlist m;
        m.path=obj.at("path").as_string().c_str();
        m.value=obj.at("value").as_string().c_str();
        return m;
    }
};
class Check {
    public:
    int version;
    //key为文件路径，value为校验的sha256值
    std::vector<Pathlist> SHA256;
    Check()=default;
    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const Check& image) {
        jv=boost::json::object{
            {"version",image.version},
            {"SHA256",boost::json::value_from(image.SHA256)}
        };
    }

    friend Check tag_invoke(boost::json::value_to_tag<Check>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        Check m;
        m.version=obj.at("version").as_int64();
        m.SHA256=boost::json::value_to<std::vector<Pathlist>>(obj.at("SHA256"));
        return m;
    };
};

#endif // V1_CONFIG_H)
