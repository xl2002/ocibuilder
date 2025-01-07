#include "storage/lockfile/lastwrite.h"
// newLastWrite 函数实现
lastwrite newLastWrite() {
    // 获取 PID
    auto pid = static_cast<uint32_t>(GetCurrentProcessId());
    
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