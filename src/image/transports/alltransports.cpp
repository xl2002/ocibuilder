#include "image/transports/alltransports.h"
#include "utils/common/go/string.h"
#include "image/image_types/docker/docker_transport.h"
// std::tuple<std::string, std::string, bool> Cut(const std::string &str, char delimiter) {
//     size_t pos = str.find(delimiter);
//     if (pos == std::string::npos) {
//         return std::make_tuple(str, "", false);
//     }
//     return std::make_tuple(str.substr(0, pos), str.substr(pos + 1), true);
// }
std::shared_ptr<ImageReference_interface> ParseImageName(std::string imgName){
    std::string transportName, withinTransport;
    bool valid;
    std::tie(transportName, withinTransport, valid) = Cut(imgName, ':');

    if (!valid) {
        // std::cerr << "Invalid image name \"" + imgName + "\", expected colon-separated transport:reference" << std::endl;
        return nullptr;
    }

    auto transport = Get(transportName);
    if (transport==nullptr) {
        // std::cerr << "Invalid image name \"" + imgName + "\", unknown transport \"" + transportName + "\"" << std::endl;
        return nullptr;
        // throw myerror("Invalid image name " + imgName + " , unknown transport " + transportName);
    }
    std::shared_ptr<ImageReference_interface> ref;
    std::tie(ref, std::ignore) =ParseReference(withinTransport);
    return ref;
}

std::shared_ptr<ImageTransport_interface> TransportFromImageName(std::string imgName){
    std::string transportName, withinTransport;
    bool valid;
    std::tie(transportName, withinTransport, valid) = Cut(imgName, ':');
    if(valid){
        return Get(transportName);
    }
    return nullptr;
}