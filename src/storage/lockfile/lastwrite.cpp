#include "storage/lockfile/lastwrite.h"
// newLastWrite 函数实现
/**
 * @brief 生成具有全局唯一性的写入标识符
 * @return 包含唯一标识特征值的 lastwrite 结构体
 * @details 通过组合以下要素保证唯一性：
 * 1. 时间戳：高精度系统时钟的纳秒计数 (8 bytes)
 * 2. 原子计数器：进程内严格递增的64位序列号 (8 bytes)
 * 3. 进程标识：当前进程的系统级 PID (4 bytes)
 * 4. 随机填充：初始化向量的可变熵源 (剩余字节)
 *
 * @note 技术特征：
 * - 多线程安全 (原子计数器保证)
 * - 跨平台兼容性 (Windows/Unix PID获取适配)
 * - 防碰撞设计 (三重复合唯一因子)
 * 
 * @warning 极限场景下可能出现： 
 * - PID重复导致纳秒级时间戳碰撞时理论存在重复可能
 */
lastwrite newLastWrite() {
    // 获取 PID
    // auto pid = static_cast<uint32_t>(GetCurrentProcessId());
    #ifdef _WIN32
    auto pid = static_cast<uint32_t>(GetCurrentProcessId());
    #else
    auto pid = static_cast<uint32_t>(getpid());
    #endif
    // 获取当前时间（纳秒）
    auto tm = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
    
    // 获取并增加计数器
    uint64_t counter = lastWriterIDCounter.fetch_add(1) + 1;

    // 创建结果缓冲区
    std::vector<unsigned char> res(lastWriterIDSize);

    // 将数据写入缓冲区
    std::memcpy(res.data(), &tm, sizeof(tm)); // 写入时间
    std::memcpy(res.data() + 8, &counter, sizeof(counter)); // 写入计数器
    std::memcpy(res.data() + 16, &pid, sizeof(pid)); // 写入 PID

    // 生成随机部分
    boost::random::random_device rng;
    boost::random::uniform_int_distribution<unsigned char> dist(0, 255);
    for (size_t i = 20; i < lastWriterIDSize; ++i) {
        res[i] = dist(rng);
    }
    return lastwrite(res);
}