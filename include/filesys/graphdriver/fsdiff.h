#if !defined(FILESYS_GRAPHDRIVER_FSDIFF_H)
#define FILESYS_GRAPHDRIVER_FSDIFF_H
#include <string>
#include <memory>
#include "filesys/graphdriver/driver.h"

// NaiveDiffDriver 类定义
class NaiveDiffDriver : public ProtoDriver_interface, public DiffDriver_interface {
public:
    // 构造函数，接受 shared_ptr 参数
    // ~NaiveDiffDriver(){
    //     std::cout << "Destroying NaiveDiffDriver" << std::endl;
    //     protoDriver.reset(); // 手动释放资源
    //     layerIDMapUpdater.reset();
    //     std::cout << "NaiveDiffDriver destroyed" << std::endl;
    // };
    ~NaiveDiffDriver()=default;
    NaiveDiffDriver()=default;
    // NaiveDiffDriver(std::shared_ptr<ProtoDriver_interface> protoDriver,
    //                 std::shared_ptr<LayerIDMapUpdater_interface> updater)
    //     : protoDriver(protoDriver ? protoDriver : throw std::invalid_argument("Invalid protoDriver")), 
    //     layerIDMapUpdater(updater ? updater : throw std::invalid_argument("Invalid updater")) {}

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

    // std::shared_ptr<ProtoDriver_interface> protoDriver=nullptr; // 使用 shared_ptr 类型
    // std::shared_ptr<LayerIDMapUpdater_interface> layerIDMapUpdater=nullptr; // 使用 shared_ptr 类型
};



#endif