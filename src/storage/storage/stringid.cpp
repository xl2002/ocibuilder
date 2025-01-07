#include "storage/storage/stringid.h"
// 辅助函数：将字节数组转换为十六进制字符串
std::string encodeToHex(const std::vector<uint8_t>& bytes) {
    std::ostringstream oss;
    for (uint8_t byte : bytes) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}

// 辅助函数：截断 ID，仅保留前 12 个字符
std::string truncateID(const std::string& id) {
    return id.substr(0, 12);
}

// 生成随机 ID 的函数
std::string generateID() {
    // 使用时间戳作为种子
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    // 初始化随机数引擎
    std::mt19937 rng(static_cast<unsigned int>(millis));
    std::uniform_int_distribution<uint8_t> dist(0, 255);

    std::vector<uint8_t> buffer(32); // 32 字节缓冲区

    while (true) {
        // 填充随机字节
        for (auto& byte : buffer) {
            byte = dist(rng);
        }

        // 将随机字节编码为十六进制字符串
        std::string id = encodeToHex(buffer);

        // 检查截断后的 ID 是否为纯数字
        std::string truncatedID = truncateID(id);
        bool isNumeric = std::all_of(truncatedID.begin(), truncatedID.end(), ::isdigit);

        // 如果截断 ID 为纯数字，则重新生成
        if (isNumeric) {
            continue;
        }

        // 返回生成的随机 ID
        return id;
    }
}

std::string GenerateRandomID() {
    return generateID();
}