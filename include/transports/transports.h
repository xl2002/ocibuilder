#if !defined(TRANS_TRANSPORTS_H)
#define TRANS_TRANSPORTS_H
#include <memory>
#include <string>
#include <vector>
#include "types/types.h"
#include "cobra/error.h"
#include <map>
#include <mutex>
class knownTransports{
    public:
    std::map<std::string,std::shared_ptr<ImageTransport>> transports;
    std::mutex mu;
    std::shared_ptr<ImageTransport> Get(std::string name);
    void Remove(std::string k);
    void Add(std::shared_ptr<ImageTransport> t);
};


std::shared_ptr<ImageTransport> Get(std::string name);

#endif // TRANS_TRANSPORTS_H
