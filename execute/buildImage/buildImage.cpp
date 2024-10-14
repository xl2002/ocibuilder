#include "buildImage/buildImage.h"

// 跨平台创建目录的函数
bool createDirectory(const std::string& path) {
#if defined(_WIN32) || defined(_WIN64)
    // Windows 平台需要包含 <windows.h>
    return CreateDirectoryA(path.c_str(), nullptr) || GetLastError() == ERROR_ALREADY_EXISTS;
#else
    // Linux/Unix 平台使用 mkdir
    return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
#endif
}

// 递归创建目录
bool createDirectories(const std::string& path) {
    size_t pos = 0;
    std::string currentPath;

    // 如果路径长度足够且是驱动器
    if (path.length() >= 2 && path[1] == ':') {
        // 从驱动器后的第一个字符开始
        pos = 2; 
    } else {
        return false; // 如果路径格式不正确，返回 false
    }
    // 创建驱动器部分（例如 D:）
    std::string drive = path.substr(0, 2);
    createDirectory(drive); // 只需确保驱动器存在

    // 循环查找并创建子目录
    while (pos < path.length()) {
        // 查找下一个分隔符
        size_t nextPos = path.find_first_of("/\\", pos);
        currentPath = path.substr(0, nextPos); // 获取当前路径

        // 仅在 currentPath 有效时创建目录
        if (!currentPath.empty() && currentPath != drive) {
            if (!createDirectory(currentPath)) {
                std::cerr << "Failed to create directory: " << currentPath << std::endl;
                return false;
            }
        }

        pos = (nextPos == std::string::npos) ? path.length() : nextPos + 1; // 移动到下一个字符
    }

    return true;
}

// 为 repositories 中的镜像创建文件结构
void createImageStructure(const std::string& imageName, const std::string& basePath) {
    // 定义镜像的目录结构
    std::string imageBasePath = basePath + imageName;
    std::vector<std::string> directories = {
        imageBasePath + "/_manifests/revisions/sha256",
        imageBasePath + "/_manifests/tags/v1.0",
        imageBasePath + "/_manifests/tags/v1.1",
        imageBasePath + "/_layers/sha256",
        imageBasePath + "/_configs/sha256"
    };

    // 创建目录结构
    for (const auto& dir : directories) {
        if (!createDirectories(dir)) {
            std::cerr << "Error creating directory structure: " << dir << std::endl;
            return;
        }
    }
    std::cout << "Structure for image '" << imageName << "' created successfully!" << std::endl;
}

// 创建 OCI Registry 的主要文件结构
void createOCIRegistryStructure(const std::string& basePath) {
    // 定义顶层的目录结构
    std::vector<std::string> topLevelDirectories = {
        basePath + "/blobs/sha256",
        basePath + "/repositories"
    };

    // 创建顶层目录结构
    for (const auto& dir : topLevelDirectories) {
        if (!createDirectories(dir)) {
            std::cerr << "Error creating top-level directory: " << dir << std::endl;
            return;
        }
    }

    // 为两个镜像创建文件结构
    createImageStructure("my-image", basePath + "/repositories/");
    createImageStructure("another-image", basePath + "/repositories/");

    std::cout << "OCI registry directory structure created successfully!" << std::endl;
}

// 创建指定文件
void createFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (file.is_open()) {
        file << content;
        file.close();
    } else {
        std::cerr << "Error creating file: " << filePath << std::endl;
    }
}

void createConf(const std::string& basePath) {
    std::string containersconfContent = R"({
    "version": 1
})";
    std::string registersconfContent = R"({
    "version": 1
})";
    std::string storageconfContent = R"({
    "version": 1
})";

    createFile(basePath + "/containers.conf", containersconfContent);
    createFile(basePath + "/registers.conf", registersconfContent);
    createFile(basePath + "/storage.conf", storageconfContent);
}
// 创建 layout 文件和 index 文件
void createLayoutFiles(const std::string& basePath) {
    std::string indexJsonContent = R"({
    "schemaVersion": 2,
    "manifests": []
})";

    std::string ociLayoutContent = R"({
    "imageLayoutVersion": "1.0.0"
})";

    createFile(basePath + "/index.json", indexJsonContent);
    createFile(basePath + "/oci-layout", ociLayoutContent);
}

// 创建指定 SHA-256 哈希值的二进制文件
void createBinaryFile(const std::string& dirPath, const std::string& hashValue, const std::vector<unsigned char>& data) {
    std::string filePath = dirPath + "/" + hashValue;
    std::ofstream file(filePath, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();
        std::cout << "Binary file created: " << filePath << std::endl;
    } else {
        std::cerr << "Error creating binary file: " << filePath << std::endl;
    }
}
// // 示例：创建一个 SHA-256 文件
// std::vector<unsigned char> binaryData = { /* 填入二进制数据 */ };
// std::string hashValue = computeSHA256(binaryData);
// createBinaryFile(basePath + "/blobs/sha256", hashValue, binaryData);


void buildOCIImage(){
    
    std::string basePath = "D:/oci-image";  // 根目录
    if (createDirectories(basePath)) {
        createConf(basePath);
        createOCIRegistryStructure(basePath + "/oci-registry");  // 创建 OCI Registry 的主要文件结构
        createLayoutFiles(basePath + "/oci-registry");  // 创建 layout 和 index 文件
    } else {
        std::cerr << "Error creating base directory." << std::endl;
    }

}