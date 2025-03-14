#if !defined(IMAGE_TRANSPORTS_TRANSPORTS_H)
#define IMAGE_TRANSPORTS_TRANSPORTS_H
#include <memory>
#include <string>
#include <vector>
#include "image/types/types.h"
#include "utils/common/error.h"
#include <map>
#include <mutex>
// class ImageReference_interface;
// class ImageTransport_interface;
class knownTransports{
    public:
    std::map<std::string,std::shared_ptr<ImageTransport_interface>> transports;
    std::mutex mu;
    std::shared_ptr<ImageTransport_interface> get(std::string name);
    void Remove(std::string k);
    void Add(std::shared_ptr<ImageTransport_interface> t);
};

extern std::shared_ptr<knownTransports> kt;
void Register(std::shared_ptr<ImageTransport_interface> t);
std::shared_ptr<ImageTransport_interface> Get(std::string name);
std::string ImageName(std::shared_ptr<ImageReference_interface> ref);
#endif // TRANS_TRANSPORTS_H
