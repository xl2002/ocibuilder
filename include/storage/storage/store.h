#if !defined(STORAGE_STORAGE_STORE_H)
#define STORAGE_STORAGE_STORE_H
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <tuple>
#include "storage/storage/layers.h"
#include "storage/storage/storage.h"
class Layer;
class LayerOptions;
class rwLayerStore_interface{
    public:
    virtual ~rwLayerStore_interface()=default;
    virtual std::tuple<std::shared_ptr<Layer>,int64_t> create(
        std::string& id,std::shared_ptr<Layer> parentLayer,std::vector<std::string>names,
        std::string mountLabel,std::map<std::string,std::string> options,
        std::shared_ptr<LayerOptions>moreOptions,bool writeable,
        std::ifstream& diff)=0;
        virtual bool savelayer()=0;
};

#endif // STORAGE_STORAGE_STORE_H)