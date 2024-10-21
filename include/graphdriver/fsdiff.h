#if !defined(GRAPHDRIVER_FSDIFF_H)
#define GRAPHDRIVER_FSDIFF_H
#include <string>
#include <memory>
#include "graphdriver/driver.h"

// NaiveDiffDriver 类定义
class NaiveDiffDriver : public ProtoDriver_interface, public DiffDriver_interface {
public:
    // 构造函数，接受 shared_ptr 参数
    NaiveDiffDriver(std::shared_ptr<ProtoDriver_interface> protoDriver,
                    std::shared_ptr<LayerIDMapUpdater_interface> updater)
        : protoDriver(protoDriver), layerIDMapUpdater(updater) {}

    // 实现 ProtoDriver_interface 的 String 方法
    std::string String() override {
        return "NaiveDiffDriver"; // 示例实现
    }

    // 实现 DiffDriver_interface 的 Method2
    void Method2() override {
        // Method2 的具体实现
    }

    // 在这里实现其他方法...
    // void Diff(...);
    // void Changes(...);
    // void ApplyDiff(...);
    // void DiffSize(...);

private:
    std::shared_ptr<ProtoDriver_interface> protoDriver; // 使用 shared_ptr 类型
    std::shared_ptr<LayerIDMapUpdater_interface> layerIDMapUpdater; // 使用 shared_ptr 类型
};

// // NewNaiveDiffDriver 函数实现
// std::shared_ptr<NaiveDiffDriver> NewNaiveDiffDriver(ProtoDriver_interface* driver, LayerIDMapUpdater_interface* updater) {
//     return std::make_shared<NaiveDiffDriver>(driver, updater);
// }

#endif