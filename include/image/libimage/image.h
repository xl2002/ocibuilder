#if !defined(IMAGE_LIBIMAGE_IMAGE_H)
#define IMAGE_LIBIMAGE_IMAGE_H
#include "image/libimage/runtime.h"
#include "image/types/types.h"
#include "image/image_types/v1/config.h"
#include "image/libimage/inspect.h"
// #include "image/types/types.h"
class Runtime;
// struct ::Image;
namespace LibImage{
    // struct RootFS;
    // Image 结构体定义
    struct Image {
        // ListData 被 (*Runtime).ListImages() 设置。注意，数据可能过时。
        struct ListData {
            // Dangling 表示图像是否悬空。使用 `IsDangling()` 来计算最新状态。
            std::shared_ptr<bool> IsDangling; // 使用智能指针以避免内存泄漏
            // Parent 指向父图像。使用 `Parent()` 来计算最新状态。
            std::shared_ptr<LibImage::Image> Parent; // 使用智能指针以避免内存泄漏
        } listData;

        // 指向运行时的反向指针。
        std::shared_ptr<Runtime> runtime=std::make_shared<Runtime>();

        // 在本地容器存储中的对应图像。
        std::shared_ptr<storage::Image> storageImage=std::make_shared<storage::Image>(); // 使用智能指针以避免内存泄漏

        // 指向容器存储的图像引用。
        std::shared_ptr<ImageReference_interface> storageReference=nullptr;

        // 以下结构中的所有字段都是缓存的。可以随时清除。
        struct Cached {
            // 图像源。出于性能原因缓存。
            std::shared_ptr<ImageSource_interface> imageSource=nullptr;

            // 从 containers/image 获得的检查数据。
            std::shared_ptr<ImageInspectInfo> partialInspectData=std::make_shared<ImageInspectInfo>(); // 使用智能指针以避免内存泄漏

            // 完整组装的图像数据。
            std::shared_ptr<ImageData> completeInspectData; // 假设的类型

            // 相应的 OCI 图像。
            std::shared_ptr<v1::Image> ociv1Image=std::make_shared<v1::Image>(); // 使用智能指针以避免内存泄漏

            // Names() 解析成引用。
            std::vector<Reference_interface> namesReferences;

            // 计算 Size() 的成本很高，因此缓存它。
            std::shared_ptr<int64_t> size; // 使用智能指针以避免内存泄漏
        } cached;

        // 构造函数
        Image()=default;

        // 其他需要的成员函数可以在此添加，例如 reload() 等
        std::shared_ptr<ImageInspectInfo> inspectInfo();
        std::shared_ptr<ImageReference_interface> StorageReference();
        std::shared_ptr<ImageData> Inspect (std::shared_ptr<InspectOptions> options);
        std::string ID();
        bool matchesPlatform( std::string os, std::string arch, std::string variant);
        bool isCorrupted(std::string name);
    };
}


#endif // LIBIMAGE_IMAGE_H)
