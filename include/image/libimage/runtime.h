#if !defined(LIBIMAGE_RUNTIME_H)
#define LIBIMAGE_RUNTIME_H
#include "define/types.h"
#include <memory>
#include <queue>
#include "storage/storage.h"
#include "libimage/events.h"
#include "libimage/pull.h"
#include "define/platform.h"
#include "libimage/inspect.h"
#include "libimage/image.h"
#include "reference/reference.h"
#include "storage/storage_transport.h"
// Runtime 负责管理镜像并将其存储在容器存储中
struct LookupImageOptions;
namespace LibImage{//由于头文件相互包含，提前声明
    struct Image;
}
struct Runtime {
    // 用于向用户发送事件的通道
    std::queue<std::shared_ptr<Event>> eventChannel; // 使用队列代替通道
    // 底层存储
    std::shared_ptr<Store> store=std::make_shared<Store>();
    // 全局系统上下文。使用值而非指针以简化复制和修改
    std::shared_ptr<SystemContext> systemContext=std::make_shared<SystemContext>();
    std::vector<std::shared_ptr<LibImage::Image>> Pull(std::string name,std::shared_ptr<PullPolicy> pullPolicy,std::shared_ptr<PullOptions> options);
    std::vector<std::string> copyFromRegistry(std::shared_ptr<ImageReference_interface> ref,std::string inputName,std::shared_ptr<PullPolicy> pullPolicy,std::shared_ptr<PullOptions> options);
    std::vector<std::string> copySingleImageFromRegistry(std::string imageName,std::shared_ptr<PullPolicy> pullPolicy,std::shared_ptr<PullOptions> options);
    std::tuple<std::shared_ptr<LibImage::Image>,std::string> LookupImage(std::string name,std::shared_ptr<LookupImageOptions> options);
    std::shared_ptr<LibImage::Image> lookupImageInLocalStorage(std::string name,std::string candidate,std::shared_ptr<Named_interface> namedCandidate,std::shared_ptr<LookupImageOptions> options);
    std::shared_ptr<LibImage::Image> storageToImage(std::shared_ptr<storage::Image> img,std::shared_ptr<ImageReference_interface> ref);
};
struct RuntimeOptions {
    std::shared_ptr<::SystemContext> SystemContext=std::make_shared<::SystemContext>();
};
// LookupImageOptions 结构体定义
struct LookupImageOptions {
    // 查找匹配指定架构的镜像
    std::string Architecture;

    // 查找匹配指定操作系统的镜像
    std::string OS;

    // 查找匹配指定变体的镜像
    std::string Variant;

    // 控制检查镜像平台时的行为
    std::shared_ptr<::PlatformPolicy> PlatformPolicy=std::make_shared<::PlatformPolicy>(platformPolicy::PlatformPolicyDefault);

    // 如果设置，不查找匹配当前平台的清单列表中的项目/实例，而是返回清单列表本身
    bool lookupManifest=false;

    // 如果匹配的镜像解析为清单列表，则返回清单列表，而不是解析为镜像实例
    // 如果找不到清单列表，则尝试解析镜像
    bool ManifestList=false;

    // 如果镜像解析为清单列表，通常我们查找匹配的实例
    // 如果找不到，则返回清单列表。此选项在镜像删除时是必需的
    bool returnManifestIfNoInstance=false;
};


void setRegistriesConfPath(std::shared_ptr<SystemContext> systemContext);
std::shared_ptr<Runtime> RuntimeFromStore(std::shared_ptr<Store> store,std::shared_ptr<RuntimeOptions> options);

#endif // LIBIMAGE_RUNTIME_H)
