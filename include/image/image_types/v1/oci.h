#if !defined(IMAGE_IMAGE_TYPES_V1_OCI_H)
#define IMAGE_IMAGE_TYPES_V1_OCI_H
#include "image/types/types.h"
#include "image/image_types/v1/config.h"
#include <boost/json.hpp>
#include "image/types/define/types.h"
class LayerInfo{
    public:
    std::shared_ptr<::BlobInfo> BlobInfo=std::make_shared<::BlobInfo>();
    bool EmptyLayer=false;
    LayerInfo()=default;
};
class OCI1:public Manifest{
    public:
    OCI1()=default;
    std::vector<LayerInfo> LayerInfos();
    /**
     * @brief 序列化
     * 
     * @param jv 
     * @param image 
     */
    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const OCI1& image) {
        jv=boost::json::object{
            {"schemaVersion",image.SchemaVersion},
            {"mediaType",image.MediaType},
            {"config",boost::json::value_from(image.Config)},
            {"layers",boost::json::value_from(image.Layers)},
            {"annotations", boost::json::value_from(image.Annotations)}
        };
    }
    /**
     * @brief 反序列化
     * 
     * @param jv 
     * @param image 
     */
    friend OCI1 tag_invoke(boost::json::value_to_tag<OCI1>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        OCI1 m;
        m.SchemaVersion=obj.at("schemaVersion").as_int64();
        m.MediaType=obj.at("mediaType").as_string().c_str();
        m.Config=boost::json::value_to<Descriptor>(obj.at("config"));
        m.Layers=boost::json::value_to<std::vector<Descriptor>>(obj.at("layers"));
        m.Annotations=boost::json::value_to<std::map<std::string, std::string>>(obj.at("annotations"));
        return m;
    }
};

class manifestOCI1{
    public:
    std::shared_ptr<ImageReference_interface> src=nullptr;
    std::vector<uint8_t> configBlob;
    std::shared_ptr<OCI1> m=std::make_shared<OCI1>();
};

std::shared_ptr<OCI1> OCI1FromManifest(std::vector<uint8_t> manifestBlob);

std::shared_ptr<OCI1> FromBlob(std::vector<uint8_t> manblob, std::string mt);
std::shared_ptr<::BlobInfo> BlobInfoFromOCI1Descriptor(std::shared_ptr<Descriptor> desc);
#endif // IMAGE_IMAGE_TYPES_V1_OCI_H
