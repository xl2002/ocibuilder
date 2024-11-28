#if !defined(IMAGE_LIBIMAGE_EVENTS_H)
#define IMAGE_LIBIMAGE_EVENTS_H
#include <stdexcept>
#include <string> 
#include <vector>
#include <memory>
#include <chrono>
enum class eventType {
    // EventTypeUnknown 是未初始化的 EventType
    EventTypeUnknown = 0,
    // EventTypeImagePull 表示镜像拉取
    EventTypeImagePull,
    // EventTypeImagePullError 表示镜像拉取失败
    EventTypeImagePullError,
    // EventTypeImagePush 表示镜像推送
    EventTypeImagePush,
    // EventTypeImageRemove 表示镜像移除
    EventTypeImageRemove,
    // EventTypeImageLoad 表示镜像正在加载
    EventTypeImageLoad,
    // EventTypeImageSave 表示镜像正在保存
    EventTypeImageSave,
    // EventTypeImageTag 表示镜像正在标记
    EventTypeImageTag,
    // EventTypeImageUntag 表示镜像正在取消标记
    EventTypeImageUntag,
    // EventTypeImageMount 表示镜像正在挂载
    EventTypeImageMount,
    // EventTypeImageUnmount 表示镜像正在卸载
    EventTypeImageUnmount
};
struct EventType {
    eventType Type=eventType::EventTypeUnknown;
};
// Event 代表事件，例如镜像拉取或镜像标记
struct Event {
    // 对象的ID（例如，镜像ID）
    std::string ID;
    // 对象的名称（例如，镜像名称 "quay.io/containers/podman:latest"）
    std::string Name;
    // 事件时间
    std::chrono::system_clock::time_point Time; // 使用 std::chrono 处理时间
    // 事件类型
    std::shared_ptr<EventType> Type=std::make_shared<EventType>();
    // 失败时的错误信息
    std::shared_ptr<std::exception> Error=nullptr; // 使用 std::string 表示错误信息
};


#endif // LIBIMAGE_EVENTS_H)