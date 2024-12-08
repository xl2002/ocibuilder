#if !defined(IMAGE_IMAGE_TYPES_V1_OCI_H)
#define IMAGE_IMAGE_TYPES_V1_OCI_H
#include "image/types/types.h"
#include "image/image_types/v1/config.h"
#include <boost/json.hpp>
#include "image/types/define/types.h"
struct LayerInfo{
    std::shared_ptr<::BlobInfo> BlobInfo=std::make_shared<::BlobInfo>();
    bool EmptyLayer=false;
    LayerInfo()=default;
};
struct OCI1:public Manifest{
    OCI1()=default;
    std::vector<LayerInfo> LayerInfos();
    /**
     * @brief 序列化
     * 
     * @param jv 
     * @param image 
     */
    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const OCI1& image) {
        
    }
    /**
     * @brief 反序列化
     * 
     * @param jv 
     * @param image 
     */
    friend void tag_invoke(boost::json::value_to_tag<OCI1>, const boost::json::value& jv, OCI1& image) {
        
    }
};

struct manifestOCI1{
    std::shared_ptr<ImageReference_interface> src=nullptr;
    std::vector<uint8_t> configBlob;
    std::shared_ptr<OCI1> m=std::make_shared<OCI1>();
};

std::shared_ptr<OCI1> OCI1FromManifest(std::vector<uint8_t> manifestBlob);

std::shared_ptr<OCI1> FromBlob(std::vector<uint8_t> manblob, std::string mt);
std::shared_ptr<::BlobInfo> BlobInfoFromOCI1Descriptor(std::shared_ptr<Descriptor> desc);
#endif // IMAGE_IMAGE_TYPES_V1_OCI_H
