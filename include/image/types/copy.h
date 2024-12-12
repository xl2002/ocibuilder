#if !defined(IMAGE_TYPES_COPY_H)
#define IMAGE_TYPES_COPY_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>
#include <chrono>
#include "image/types/internal/types.h"
#include "utils/common/buffer.h"
#include "image/types/signer.h"
#include "image/types/reference/reference.h"
#include "image/types/define/types.h"
#include "config/config.h"
#include "utils/common/semaphore.h"
#include "image/libimage/copier.h"
#include "image/types/signature/policy_eval.h"
#include "image/libimage/single.h"
namespace copy{
    struct ImageListSelection{
        int imagelistselection=0;
    };
    struct OptionCompressionVariant{
        std::shared_ptr<Algorithm> Algorithm=std::make_shared<::Algorithm>();
        std::shared_ptr<int> Level=std::make_shared<int>();
    };
    // CopyImage的选项结构体
    struct Options {
        bool removeSignatures=false;                                // 是否移除已有的签名
        std::vector<std::shared_ptr<Signer>> signers;                         // 用于添加签名的签名器
        std::string signBy;                                   // 要使用的签名的密钥ID
        std::string signPassphrase;                           // 签名时的密码
        std::string signBySigstorePrivateKeyFile;             // 使用sigstore私钥文件进行签名的路径
        std::vector<byte> signSigstorePrivateKeyPassphrase;   // 使用sigstore私钥文件签名时的密码
        std::shared_ptr<Named_interface> signIdentity=nullptr;                         // 签名时的标识

        std::ostream* reportWriter=nullptr;                           // 报告输出流
        std::shared_ptr<SystemContext> sourceCtx=std::make_shared<SystemContext>();                             // 源上下文
        std::shared_ptr<SystemContext> destinationCtx=std::make_shared<SystemContext>();                        // 目标上下文
        std::chrono::duration<int64_t> progressInterval;      // 进度报告间隔时间
        std::shared_ptr<std::vector<ProgressProperties>> progress=std::make_shared<std::vector<ProgressProperties>>(); // 进度报告的通道

        bool preserveDigests=false;                                 // 是否保留摘要，并在失败时中止
        std::string forceManifestMIMEType;                    // 用户设置的镜像manifest MIME类型
        std::shared_ptr<ImageListSelection> imageListSelection=std::make_shared<ImageListSelection>();                // 镜像列表选择
        std::vector<Digest> instances;                        // 当选择具体镜像时，要复制的实例列表

        // 优先使用gzip压缩实例
        OptionalBool preferGzipInstances; 

        std::shared_ptr<EncryptConfig> ociEncryptConfig=std::make_shared<EncryptConfig>();                      // 用于加密镜像的配置
        std::vector<int> ociEncryptLayers;                   // 指定要加密的层
        std::shared_ptr<DecryptConfig> ociDecryptConfig=std::make_shared<DecryptConfig>();                      // 用于解密镜像的配置

        std::shared_ptr<Weighted> concurrentBlobCopiesSemaphore=std::make_shared<Weighted>();                 // 限制并发复制层和配置的信号量
        uint32_t maxParallelDownloads=0;                        // 最大并行下载数

        bool optimizeDestinationImageAlreadyExists=false;           // 优化复制操作，假设目标镜像已存在
        bool downloadForeignLayers=false;                           // 下载不可分发的"外部"层，并将其转换为可分发格式

        std::vector<OptionCompressionVariant> ensureCompressionVariantsExist; // 确保压缩变体存在
        bool forceCompressionFormat=false;                          // 强制使用指定的压缩格式
    };
}
std::vector<uint8_t> Image(std::shared_ptr<PolicyContext>policyContext,std::shared_ptr<ImageReference_interface> destRef,std::shared_ptr<ImageReference_interface> srcRef,std::shared_ptr<ImageReference_interface> registry,std::shared_ptr<copy::Options> copyOptions);
struct UnparsedImage{
    std::shared_ptr<ImageReference_interface> src=nullptr;
    std::shared_ptr<Digest> digest=std::make_shared<Digest>();
    std::vector<uint8_t> cachedManifest;
    std::string cachedManifestMIMEType;
    // cachedSignatures       []signature.Signature
    UnparsedImage()=default;
};
struct copier{
    std::shared_ptr<PolicyContext> policyContext=std::make_shared<PolicyContext>();
    std::shared_ptr<ImageDestination_interface> dest=nullptr;
    std::shared_ptr<ImageSource_interface> rawSource=nullptr;
    std::shared_ptr<copy::Options> options=std::make_shared<copy::Options>();
    std::shared_ptr<std::ostream> reportWriter=nullptr;
    std::shared_ptr<std::ostream> progressOutput=nullptr;
    std::shared_ptr<UnparsedImage> unparsedToplevel=nullptr;
    copier()=default;
    std::shared_ptr<copySingleImageResult> copySingleImage(std::shared_ptr<UnparsedImage> unparsedImage,std::shared_ptr<Digest> targetInstance,std::shared_ptr<copySingleImageOptions> opts);
};
struct manifestConversionPlan{
    std::string preferredMIMEType;
    bool preferredMIMETypeNeedsConversion=false;
    std::vector<std::string> otherMIMETypeCandidates;
    manifestConversionPlan()=default;
};
struct imageCopier{
    std::shared_ptr<copier> c=std::make_shared<copier>();
    std::shared_ptr<ManifestUpdateOptions> manifestUpdates=std::make_shared<ManifestUpdateOptions>();
    std::shared_ptr<ImageSource_interface> src=nullptr;
    std::shared_ptr<::manifestConversionPlan> manifestConversionPlan=std::make_shared<::manifestConversionPlan>();
    bool diffIDsAreNeeded=false;
    std::string cannotModifyManifestReason;
    bool canSubstituteBlobs=false;
    std::shared_ptr<Algorithm> compressionFormat=std::make_shared<Algorithm>();
    std::shared_ptr<int> compressionLevel=std::make_shared<int>(0);
    bool requireCompressionFormatMatch=false;

    imageCopier()=default;
    std::shared_ptr<Algorithm> copyLayers();
    std::tuple<std::shared_ptr<BlobInfo>,std::shared_ptr<Digest>> copyLayer(std::shared_ptr<BlobInfo> srcInfo,bool toEncrypt,int layerIndex,std::shared_ptr<Named_interface> srcRef,bool emptyLayer);
    // std::shared_ptr<bpCompressionStepData> bpcPreserveOriginal(std::shared_ptr<bpDetectCompressionStepData>detect,bool layerCompressionChangeSupported);
};

struct copyLayerData{
    std::shared_ptr<BlobInfo> destInfo=std::make_shared<BlobInfo>();
    std::shared_ptr<Digest> diffID=std::make_shared<Digest>();
    copyLayerData()=default;
};


#endif // COPY_COPY_H
