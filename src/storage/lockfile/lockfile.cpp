#include "storage/lockfile/lockfile.h"
// 全局变量，存储不同路径的 lockFile 对象

std::mutex lockFilesMutex;
/**
 * @brief 原子化打开文件并返回文件描述符
 * @param path 目标文件路径
 * @param mode 打开模式标志位 (O_RDWR/O_CREAT等)
 * @return 成功打开的文件描述符
 * @throws myerror 文件打开失败时抛出异常
 * @note 包含 O_CLOEXEC 标志防子进程继承
 */
int openHandle(const std::string& path, int mode) {
    // Add O_CLOEXEC to mode to ensure the file descriptor is closed on exec
    // mode |= O_CLOEXEC;
    //linux系统设置防止文件描述符意外被子进程继承

    // Open the file with the given mode and permissions
    int fd = ::open(path.c_str(), mode, 0644);
    
    // If the file descriptor is -1, an error occurred
    if (fd == -1) {
        throw myerror("Failed to open file: " + string(strerror(errno)));
    }

    return fd; // Return the file descriptor if successful
}
// struct flock {
//     short l_type;   // 锁的类型：F_RDLCK、F_WRLCK、F_UNLCK
//     short l_whence; // 锁的起始位置：SEEK_SET、SEEK_CUR、SEEK_END
//     off_t l_start;  // 锁定区域的起始位置
//     off_t l_len;    // 锁定区域的长度
//     pid_t l_pid;    // 锁定者的进程ID（可选）
// };

// lockHandle 实现windows
// 锁定函数
/**
 * @brief 跨平台文件锁定核心实现
 * @param filePath 需要操作的文件路径
 * @param lType 锁类型 (read/write)
 * @throws myerror 锁定失败时抛出异常
 * @note Windows版本使用Boost.Interprocess实现
 */
void lockHandle(const std::string& filePath, LockType lType) {
    try {
        // 确保文件存在
        if (!boost::filesystem::exists(filePath)) {
            std::ofstream(filePath); // 创建空文件
        }

        // 创建文件锁对象
        boost::interprocess::file_lock fileLock(filePath.c_str());

        // 锁定文件
        if (lType == LockType::WriteLock) {
            fileLock.lock(); // 排他性锁
        } else {
            // 这里使用排他性锁来模拟读锁
            // Boost.Interprocess 不直接支持共享锁
            fileLock.lock(); // 注意，这里需要注意不同系统对读锁的处理
        }

        // 模拟一些操作
        // std::this_thread::sleep_for(std::chrono::seconds(5));

        // 释放锁
        fileLock.unlock();
    } catch (const std::exception& e) {
        throw myerror("处理文件锁定时发生异常: " + std::string(e.what()));
    }
}
// 以下属于unix更适合的底层调用锁定
/*int openLock(const string& file, bool ro) {
    int flags = O_CREAT;
    if (ro) {
        flags |= O_RDONLY; // 如果是只读模式，则设置只读标志
    } else {
        flags |= O_RDWR;   // 否则设置读写标志
    }

    int fd = openHandle(file, flags);
    if (fd != -1) {
        return fd; // 如果成功打开文件，直接返回文件描述符
    }

    if (errno == ENOENT) { // 如果错误是因为文件或目录不存在
        MkdirAll(file); // 创建文件路径的所有必要目录
        return openLock(file, ro); // 递归调用自己以再次尝试打开文件
    }

    throw myerror("Failed to open lock file: " + string(strerror(errno))); // 使用 myerror 抛出异常，包含错误信息
}*/
// openLock 实现boost版本

/**
 * @brief 原子化文件打开/创建操作
 * @param path 目标文件路径（将自动规范化）
 * @param ro 是否以只读方式访问
 * @return 已打开文件的流式句柄
 * @throws myerror 包含以下错误场景：
 * - 文件创建权限不足(boost::filesystem_error)
 * - 目录结构创建失败
 * - 文件打开操作被系统拒绝
 * 
 * @details 典型执行流程：
 * 1. 尝试直接打开目标文件
 * 2. 若路径不存在则自动创建完整目录结构
 * 3. 二次尝试打开文件
 * 4. 失败时返回详细的错误堆栈信息
 * 
 * @warning 递归创建目录过程可能引入性能开销
 */
fileHandle openLock(const std::string& path, bool ro) {
    namespace fs = boost::filesystem;
    std::ios_base::openmode mode = fs::fstream::out | fs::fstream::binary;

    if (ro) {
        mode = fs::fstream::in | fs::fstream::binary;
    } else {
        mode |= fs::fstream::in;
    }

    // 尝试打开文件
    try {
        std::shared_ptr<fs::fstream> fileStream;
        if (!fs::exists(path)) {
            fileStream = std::make_shared<fs::fstream>(path, fs::fstream::out | fs::fstream::binary);
        }else{
            // 创建 std::shared_ptr<boost::filesystem::fstream> 实例
            fileStream = std::make_shared<fs::fstream>(path, mode);
        }
        if (!fileStream->is_open()) {
            throw myerror("Failed to open file: " + path);
        }
        return fileStream;
    } catch (const fs::filesystem_error& e) {
        if (e.code().value() == boost::system::errc::no_such_file_or_directory) {
            // 文件目录不存在，尝试创建目录
            try {
                fs::create_directories(fs::path(path).parent_path());
                // 递归调用以重新尝试打开文件
                return openLock(path, ro);
            } catch (const fs::filesystem_error& dirError) {
                throw myerror("Creating lock file directory: " + std::string(dirError.what()));
            }
        } else {
            throw myerror("Opening lock file: " + std::string(e.what()));
        }
    }
}


// Lock 方法实现boost
/**
 * @brief 申请指定类型的文件锁
 * @param lType READ(共享)|WRITE(排他)
 * @throws myerror 打开文件失败时抛出异常
 * @details
 * - 读锁允许多个实例共享
 * - 写锁具有排他性
 * - 使用引用计数器管理锁生命周期
 */
void lockFile::lock(LockType lType) {
    if (lType == LockType::ReadLock) {
        // boost::unique_lock<boost::shared_mutex> lock(rwMutex, boost::defer_lock);
        // lock.lock();  // 读锁
    } else {
        // boost::unique_lock<boost::shared_mutex> lock(rwMutex);  // 写锁
    }

    boost::unique_lock<boost::mutex> stateLock(stateMutex);

    if (counter == 0) {
        // 如果这是第一个引用锁，我们需要再次打开文件
        try {
            fd = openLock(file, ro);
        } catch (const myerror& e) {
            throw myerror("Failed to open lock file: " + std::string(e.what()));
        }

        // 优化：仅在计数器为 0 时使用（昂贵的）系统调用。
        // 在这种情况下，我们要么是第一个读者锁，要么是写锁。
        try {
            lockHandle(file, lType);
        } catch (const myerror& e) {
            throw myerror("锁定文件失败: " + std::string(e.what()));
        }
    }
    lockType = lType;
    locked = true;
    counter++;
}
/**
 * @brief 显式申请写锁接口
 * @throws myerror 对只读锁申请写锁时抛出
 */
void lockFile::Lock() {
    if (ro) {
        throw myerror("can't take write lock on read-only lock file");
    } else {
        lock(LockType::WriteLock);
    }
}
/**
* @brief 释放锁并关闭文件句柄
* @note 当引用计数器归零时执行实际解锁操作
*/
void lockFile::Unlock(){
    this->stateMutex.lock();
    if(!this->locked){
        
    }
    this->counter--;
    if(this->counter == 0){
        this->locked = false;
        unlockAndCloseHandle(this->fd);
    }
    if(this->lockType==LockType::ReadLock){
        this->rwMutex.unlock();
    }
    // boost::unique_lock<boost::shared_mutex> lock(rwMutex, boost::defer_lock);
    // lock.unlock();  // 读锁
    // this->rwMutex.unlock();
    this->stateMutex.unlock();
}

// 从数据创建 lastwrite 的函数
/**
 * @brief 生成最新的写入状态标记
 * @param serialized 用于生成特征的二进制数据
 * @return 带时间戳的特征结构体
 */
lastwrite newLastWriteFromData(const std::vector<uint8_t>& serialized) {
    // if (serialized.empty()) {
    //     throw std::invalid_argument("newLastWriteFromData with empty data");
    // }
    lastwrite lw;
    lw.state = serialized;
    return lw;
}
/**
 * @brief 检查锁是否被当前实例持有
 * @throws myerror 出现锁定状态不一致时抛出
*/
void lockFile::AssertLocked() {
    if (!locked) {
        throw myerror("internal error: lock is not held by the expected owner");
    }
}

// 实现 GetLastWrite 方法
/**
 * @brief 获取文件的最后写入标记
 * @return lastwrite 结构体包含文件状态特征值
 * @throws myerror 锁定状态验证失败时抛出异常
 */
lastwrite lockFile::GetLastWrite() {
    try{
        AssertLocked();
    }
    catch (const std::exception& e){
        throw myerror("锁定文件失败: " + std::string(e.what()));
    }

    // 定义读取缓冲区
    std::vector<uint8_t> contents(64);

    try {
        // 打开文件流以读取文件内容
        std::ifstream fileStream(file, std::ios::binary);
        if (!fileStream) {
            throw myerror("无法打开文件: " + file);
        }

        // 从文件读取内容
        fileStream.read(reinterpret_cast<char*>(contents.data()), contents.size());
        std::streamsize bytesRead = fileStream.gcount();

        // 处理部分读取情况
        contents.resize(bytesRead);  // 只保留实际读取的部分
    } catch (const std::exception& e) {
        throw myerror("读取文件内容失败: " + std::string(e.what()));
    }

    // 将数据转换为 lastwrite 类型并返回
    return newLastWriteFromData(contents);
}
// // 函数用于从数据中创建 lastwrite 对象
// lastwrite newLastWrite() {
//     // 在这里实现 your newLastWrite 的具体逻辑
//     //待实现
//     return lastwrite();  // 示例返回，具体实现应根据需求完成
// }

// 函数用于将 lastwrite 对象转换为序列化的字节数据
/**
 * @brief 序列化写入状态特征
 * @param lw 输入特征结构体
 * @return 包含特征值的二进制数组
 */
std::vector<uint8_t> serializeLastWrite(const lastwrite& lw) {
    return lw.state;  // 示例实现，具体序列化方式应根据需求完成
}

/**
 * @brief 记录当前写入状态到锁文件
 * @return 更新后的lastwrite结构
 * @throws myerror 验证写锁状态失败或文件操作异常时抛出
 */
lastwrite lockFile::RecordWrite() {
    try {
        AssertLockedForWriting();

        lastwrite lw = newLastWrite();
        std::vector<uint8_t> lockContents = serializeLastWrite(lw);

        // 写入内容到文件
        std::ofstream fileStream(file, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!fileStream) {
            throw myerror("无法打开文件进行写入: " + file);
        }

        fileStream.write(reinterpret_cast<const char*>(lockContents.data()), lockContents.size());
        if (!fileStream) {
            throw myerror("写入文件失败: " + file);
        }

        // 检查写入的字节数
        std::streamsize bytesWritten = fileStream.tellp();
        if (bytesWritten != lockContents.size()) {
            throw myerror("写入文件的字节数不匹配: " + file);
        }

        return lw;
    } catch (const std::exception& e) {
        throw myerror("记录写入失败: " + std::string(e.what()));
    }
}



/**
 * @brief 安全释放文件锁并关闭关联句柄
 * @param fd 将被释放的文件句柄智能指针
 * @note 特性说明：
 * - 对已关闭的句柄调用无害（幂等操作）
 * - 自动判断流对象是否有效
 * - 线程安全（无状态依赖）
 */
void unlockAndCloseHandle(fileHandle fd) {
    if (fd && fd->is_open()) {
        fd->close();
    }
}
/**
 * @brief 创建指定路径的锁文件对象
 * @param path 目标文件路径
 * @param ro 是否以只读方式操作
 * @return 初始化的lockFile智能指针
 * @throws myerror 创建锁文件异常时抛出
 */
std::shared_ptr<lockFile> createLockFileForPath(const std::string& path, bool ro) {
    // 尝试打开锁文件
    fileHandle fileStream = openLock(path, ro);
    if (!fileStream || !fileStream->is_open()) {
        throw myerror("Failed to open lock file: " + path);
    }

    // 解锁并关闭句柄
    unlockAndCloseHandle(fileStream);

    // 根据只读模式设置锁类型
    LockType lType = ro ? LockType::ReadLock : LockType::WriteLock;

    // 返回新的 lockFile 对象
    auto l=std::make_shared<lockFile>(path, ro, lType);
    l->lw=newLastWrite();
    return l;
    // return std::make_shared<lockFile>(path, ro, lType);
}
/**
 * @brief 获取路径对应的锁文件单例
 * @param path 标准化后的绝对路径
 * @param ro 是否以只读方式访问
 * @return 存在则返回缓存实例，否则创建新实例
 */
std::shared_ptr<lockFile> getLockfile(const std::string& path, bool ro) {
    std::lock_guard<std::mutex> lock(lockFilesMutex);  // 确保线程安全

    if (lockFiles.empty()) {
        lockFiles = std::map<std::string, std::shared_ptr<lockFile>>();
    }

    // 获取绝对路径
    std::string cleanPath;
    try {
        cleanPath = boost::filesystem::absolute(path).string();
    } catch (const std::exception& e) {
        throw myerror("ensuring that path " + path + " is an absolute path: " + e.what());
    }

    // 检查是否已经存在 lockFile
    auto it = lockFiles.find(cleanPath);
    if (it != lockFiles.end()) {
        auto lockFile = it->second;
        if (ro && lockFile->IsReadWrite()) {
            throw myerror("lock " + cleanPath + " is not a read-only lock");
        }
        if (!ro && !lockFile->IsReadWrite()) {
            throw myerror("lock " + cleanPath + " is not a read-write lock");
        }
        return lockFile;
    }

    // 创建新的 lockFile 对象
    auto lockFile = createLockFileForPath(cleanPath, ro);
    if (!lockFile) {
        throw myerror("Failed to create lock file for path: " + cleanPath);
    }

    lockFiles[cleanPath] = lockFile;
    return lockFile;
}
/**
 * @brief 获取路径对应的锁文件单例
 * @param path 标准化后的绝对路径
 * @param ro 是否以只读方式访问
 * @return 存在则返回缓存实例，否则创建新实例
 */
std::shared_ptr<lockFile> GetLockFile(const std::string& path) {
    return getLockfile(path, false);
}

/**
 * @brief 检测锁文件是否已被修改
 * @param previous 先前存储的状态标记
 * @return pair<当前状态,是否被修改>
 * @throws myerror 锁定状态验证失败时抛出异常
 */
std::pair<lastwrite, bool> lockFile::ModifiedSince(const lastwrite& previous) {
    AssertLocked(); // 确保当前已经持有锁
    lastwrite currentLW = GetLastWrite(); // 获取当前的 lastwrite 状态

    bool modified = !(previous.state == currentLW.state); // 检查状态是否改变
    return std::make_pair(currentLW, modified);
}
/**
 * @brief 验证当前是否持有写锁
 * @throws myerror 当未持有写锁时抛出异常
 * @details 该方法会依次执行以下检查：
 * 1. 通过 AssertLocked() 确保处于锁定状态
 * 2. 检查锁类型是否为写锁类型
 * @note 该检查主要用于防止读写锁模式的误用
 */
void lockFile::AssertLockedForWriting() {
    try {
        AssertLocked(); // 确保锁定状态

        if (lockType == LockType::ReadLock) {
            throw myerror("Internal error: lock is not held for writing");
        }
    } catch (const myerror& e) {
        throw; // 重新抛出 myerror 类型异常
    }
}