#if !defined(IMAGE_IMAGE_TYPES_V1_DESCRIPTOR_H)
#define IMAGE_IMAGE_TYPES_V1_DESCRIPTOR_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "image/digest/digest.h"
#include "utils/common/buffer.h"
#include "image/image_types/v1/mediatype.h"
#include <boost/json.hpp>
class Platform{
    public:
    std::string Architecture;
    std::string OS;
    std::string OSVersion;
    std::vector<std::string> OSFeatures;
    std::string Variant;
    Platform()=default;
    Platform(std::string a,std::string o,std::string v):
        Architecture(a),OS(o),Variant(v){};
    
};

class Descriptor{
    public:
    std::string MediaType;
    Digest Digests;
    int64_t Size=0;
    std::vector<string> URLs;
    std::map<string,string> Annotations;
    std::vector<byte> Data;
    std::shared_ptr<Platform> Platforms=std::make_shared<Platform>();
    std::string ArtifactType;
    Descriptor()=default;
    Descriptor(std::string m,std::string d,int64_t s,std::vector<byte> da):
        MediaType(m),Digests(d),Size(s),Data(da){};
    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const Descriptor& image) {
        jv=boost::json::object{
            {"mediaType",image.MediaType},
            {"digest",image.Digests.digest},
            {"size",image.Size}
        };
    }
    friend Descriptor tag_invoke(boost::json::value_to_tag<Descriptor>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        Descriptor image;
        image.MediaType=obj.at("mediaType").as_string().c_str();
        image.Digests.digest=obj.at("digest").as_string().c_str();
        image.Size=obj.at("size").as_int64();
        return image;
    }
};
extern Descriptor DescriptorEmptyJSON;
#endif // V1_DESCRIPTOR_H
