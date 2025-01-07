#if !defined(FILESYS_GRAPHDRIVER_CHOWN_H)
#define FILESYS_GRAPHDRIVER_CHOWN_H

#include <string>       // for std::string
#include <memory>       // for std::shared_ptr
#include "filesys/graphdriver/driver.h"

// naiveLayerIDMapUpdater 类实现
// 假设 naiveLayerIDMapUpdater 继承自 LayerIDMapUpdater_interface
// class ProtoDriver_interface;
// class LayerIDMapUpdater_interface;
class naiveLayerIDMapUpdater : public LayerIDMapUpdater_interface {
public:
    // 构造函数接受 shared_ptr 作为参数
    naiveLayerIDMapUpdater()=default;
    naiveLayerIDMapUpdater(std::shared_ptr<ProtoDriver_interface> protoDriver)
        : protoDriver(protoDriver) {}

    void UpdateLayerIDMap(std::string& id) override {
        // 更新层 ID 的实现
    }

    std::shared_ptr<ProtoDriver_interface> protoDriver=nullptr; // 使用 shared_ptr 类型
};
// NewNaiveLayerIDMapUpdater 函数实现
std::shared_ptr<LayerIDMapUpdater_interface> NewNaiveLayerIDMapUpdater(std::shared_ptr<ProtoDriver_interface> protoDriver);

#endif
