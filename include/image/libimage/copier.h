#if !defined(LIBIMAGE_COPIER_H)
#define LIBIMAGE_COPIER_H

#include "define/types.h"
#include "libimage/manifest.h"
#include "internal/types.h"
#include "config/config.h"
#include <queue>
#include "signer/signer.h"
#include "bytes/buffer.h"
// CopyOptions 结构体定义
struct CopyOptions {
    // 如果设置，将用于复制图像。下面的字段可能会覆盖某些设置。
    std::shared_ptr<::SystemContext> SystemContext=std::make_shared<::SystemContext>();
    
    // 允许自定义源引用查找。
    LookupReferenceFunc SourceLookupReferenceFunc = nullptr;

    // 允许自定义目标引用查找。
    LookupReferenceFunc DestinationLookupReferenceFunc = nullptr;

    // 压缩格式
    std::shared_ptr<Algorithm> CompressionFormat=std::make_shared<::Algorithm>();
    
    // 压缩级别
    std::shared_ptr<int> CompressionLevel=std::make_shared<int>();

    // 确保仅使用设置的压缩算法
    bool ForceCompressionFormat = false;

    // 用于身份验证的 auth 文件路径
    std::string AuthFilePath;

    // blob-info 缓存的自定义路径
    std::string BlobInfoCacheDirPath;

    // 证书目录路径
    std::string CertDirPath;

    // 在复制到 `dir` 传输目标时强制层压缩
    bool DirForceCompress = false;

    // 允许通过 HTTP 联系注册表
    OptionalBool InsecureSkipTLSVerify;

    // 最大重试次数
    std::shared_ptr<unsigned int> MaxRetries=std::make_shared<unsigned int>();

    // 重试延迟
    std::chrono::duration<double> RetryDelay=std::chrono::seconds(0);

    // 期望的图像 MIME 类型
    std::string ManifestMIMEType;

    // 接受未压缩的层
    bool OciAcceptUncompressedLayers = false;

    // 如果 OciEncryptConfig 非空，表示图像应加密
    std::shared_ptr<EncryptConfig> OciEncryptConfig=std::make_shared<::EncryptConfig>();

    // 表示要加密的层列表
    std::shared_ptr<std::vector<int>> OciEncryptLayers;

    // 解密配置
    std::shared_ptr<DecryptConfig> OciDecryptConfig=std::make_shared<::DecryptConfig>();

    // 进度通道
    std::queue<std::shared_ptr<ProgressProperties>> Progress; // 假设是一个输出流

    // 允许使用存储传输
    bool PolicyAllowStorage = false;

    // 签名策略路径
    std::string SignaturePolicyPath;

    // 签名者列表
    std::vector<std::shared_ptr<Signer>> Signers;

    // 指定的 key ID
    std::string SignBy;

    // 签名用的 passphrase
    std::string SignPassphrase;

    // 私钥文件路径
    std::string SignBySigstorePrivateKeyFile;

    // 私钥文件的 passphrase
    std::vector<byte> SignSigstorePrivateKeyPassphrase;

    // 移除任何预先存在的签名
    bool RemoveSignatures = false;

    // 用于显示复制信息的输出流
    std::ostream* Writer = nullptr;

    // ----- platform -----------------------------------------------------

    // 选择图像的架构
    std::string Architecture;

    // 选择图像的操作系统
    std::string OS;

    // 选择图像的变体
    std::string Variant;

    // ----- credentials --------------------------------------------------

    // 注册表的用户名
    std::string Username;

    // 注册表的密码
    std::string Password;

    // 作为替代的凭证
    std::string Credentials;

    // 身份令牌
    std::string IdentityToken;

    // ----- internal -----------------------------------------------------

    // 其他标签
    std::vector<std::shared_ptr<NamedTagged_interface>> dockerArchiveAdditionalTags; // 假设的类型

    // NOTIFY_SOCKET
    std::string extendTimeoutSocket;

    // 构造函数
    CopyOptions() = default;

    // 其他需要的函数可以在此添加
};

#endif // LIBIMAGE_COPIER_H
