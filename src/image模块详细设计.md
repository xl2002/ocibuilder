# src/image 模块详细设计

## 1. 模块概述
src/image模块是OCI镜像构建和管理的核心模块，主要功能包括：
- 容器镜像构建
- 镜像摘要计算和验证
- 镜像配置和manifest处理
- 镜像层管理
- 镜像传输和存储
- 镜像引用处理

## 2. 文件功能详细说明

### 2.1 buildah/ 镜像构建
- `image.cpp`: 镜像构建核心实现，包括镜像引用创建、层处理、配置生成
- `add.cpp`: 实现向镜像添加内容的操作
- `config.cpp`: 镜像配置处理
- `digester.cpp`: 镜像摘要计算
- `mount.cpp`: 镜像挂载操作
- `push.cpp`: 镜像推送功能
- `tar.cpp`: 镜像tar包处理

### 2.2 digest/ 摘要处理
- `digest.cpp`: 摘要核心功能(编码、验证、创建)
- `algorithm.cpp`: 摘要算法实现
- `sha256.cpp`: SHA256算法具体实现
- `verifier.cpp`: 摘要验证器实现

### 2.3 image_types/ 镜像类型
- `docker_transport.cpp`: Docker镜像传输处理
- `manifest.cpp`: 镜像manifest处理
- `oci.cpp`: OCI镜像格式支持

### 2.4 libimage/ 镜像核心操作
- `image.cpp`: 镜像基础操作
- `inspect.cpp`: 镜像检查功能
- `pull.cpp`: 镜像拉取实现
- `runtime.cpp`: 镜像运行时支持

### 2.5 transports/ 镜像传输
- `alltransports.cpp`: 所有传输类型注册
- `transports.cpp`: 传输基础功能

### 2.6 types/ 类型定义
- `reference.cpp`: 镜像引用处理
- `blobcache.cpp`: blob缓存管理
- `signature/`: 镜像签名相关

## 3. 关键类和接口

### 3.1 Digest类
```cpp
class Digest {
public:
    std::string Encoded();  // 获取编码部分
    std::string Hex();      // 获取十六进制编码
    std::string String();   // 获取完整摘要字符串
    std::shared_ptr<Algorithm_sha256> Algorithm(); // 获取算法实例
    std::shared_ptr<Verifier_interface> Verifier(); // 创建验证器
    void Validate();        // 验证摘要有效性
};
```

### 3.2 镜像构建接口
```cpp
class ImageBuilder {
public:
    std::shared_ptr<ImageSource> NewImageSource();
    void createConfigsAndManifests();
    std::string computeLayerMIMEType();
    void SaveConfig();
    void UploadManifest();
};
```

## 4. 模块关系
- 依赖utils模块提供通用功能
- 被src/cmd模块调用实现镜像构建命令
- 与src/storage模块交互实现镜像存储

## 5. 设计考虑
- 支持标准OCI镜像格式
- 使用SHA256作为默认摘要算法
- 线程安全的接口设计
- 完善的错误处理机制
