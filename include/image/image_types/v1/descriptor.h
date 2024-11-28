#if !defined(IMAGE_IMAGE_TYPES_V1_DESCRIPTOR_H)
#define IMAGE_IMAGE_TYPES_V1_DESCRIPTOR_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "image/digest/digest.h"
#include "bytes/buffer.h"
#include "image/image_types/v1/mediatype.h"
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
};
extern Descriptor DescriptorEmptyJSON;
#endif // V1_DESCRIPTOR_H
