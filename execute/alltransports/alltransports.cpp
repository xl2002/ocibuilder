#include "alltransports/alltransports.h"

std::tuple<std::string, std::string, bool> Cut(const std::string &str, char delimiter) {
    size_t pos = str.find(delimiter);
    if (pos == std::string::npos) {
        return std::make_tuple(str, "", false);
    }
    return std::make_tuple(str.substr(0, pos), str.substr(pos + 1), true);
}
std::shared_ptr<ImageReference> ParseImageName(std::string imgName){
    std::string transportName, withinTransport;
    bool valid;
    std::tie(transportName, withinTransport, valid) = Cut(imgName, ':');

    if (!valid) {
        throw myerror("Invalid image name \"" + imgName + "\", expected colon-separated transport:reference");
    }

    auto transport = Get(transportName);
    if (!transport) {
        throw myerror("Invalid image name \"" + imgName + "\", unknown transport \"" + transportName + "\"");
    }

    return transport->ParseReference(withinTransport);
}