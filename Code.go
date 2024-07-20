struct SBOMScanOptions {
    std::vector<std::string> Type;    // 一个缩短的名称，表示已定义的这些选项的组
    std::string Image;                // 要使用的扫描器映像
    PullPolicy PullPolicy;           // 获取扫描器映像的方式
    std::vector<std::string> Commands; // 要用于图像根文件系统或 ContextDir 位置的一个或多个命令
    std::vector<std::string> ContextDir; // 要查找的目录位置
    std::string SBOMOutput;           // 在外部存储的 SBOM 扫描器输出（即本地文件系统）
    std::string PURLOutput;           // 在外部存储的 PURL 列表（即本地文件系统）
    std::string ImageSBOMOutput;      // 在图像中存储的 SBOM 扫描器输出
    std::string ImagePURLOutput;      // 在图像中存储的 PURL 列表
    SBOMMergeStrategy MergeStrategy;  // 多次扫描的输出合并方式
};
