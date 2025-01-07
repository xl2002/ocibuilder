#if !defined(IMAGE_IMAGE_TYPES_DOCKER_IMAGE_H)
#define IMAGE_IMAGE_TYPES_DOCKER_IMAGE_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include "image/image_types/docker/types.h"
namespace Docker{
    struct RootFS{
        std::string Type;
        std::vector<std::string> Layers;
        RootFS()=default;
    };
    // 表示 Docker 镜像及其各种属性的类
    class Image {
    public:
        std::string ID;                            // 镜像的唯一标识符
        std::vector<std::string> RepoTags;         // 镜像的标签
        std::string Parent;                        // 父镜像的 ID
        std::string Comment;                       // 评论信息
        std::chrono::system_clock::time_point Created;                       // 创建时间
        std::string Container;                     // 容器 ID
        std::shared_ptr<container_Config> ContainerConfig=std::make_shared<container_Config>();   // 容器配置
        std::string DockerVersion;                 // Docker 版本
        std::string Author;                        // 镜像作者
        std::shared_ptr<container_Config> Config=std::make_shared<container_Config>();;            // 镜像配置
        std::string Architecture;                  // 架构信息
        int64_t Size=0;                              // 镜像的实际大小
        int64_t VirtualSize=0;                       // 镜像的虚拟大小
        std::vector<std::string> RepoDigests;      // 镜像的摘要
        std::shared_ptr<Docker::RootFS> RootFS=std::make_shared<Docker::RootFS>();            // 文件系统信息
        std::string OS;                            // 操作系统信息

        // 默认构造函数
        Image()=default;
        // 禁用拷贝构造函数和赋值运算符
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

        // 定义移动构造函数和移动赋值运算符
        Image(Image&& other) noexcept = default;
        Image& operator=(Image&& other) noexcept = default;
        // 其他方法按需求实现
    };
}

#endif // DOCKER_IMAGE_H
