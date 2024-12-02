#include "storage/lockfile/lockfile.h"
// 全局变量，存储不同路径的 lockFile 对象
std::map<std::string, std::shared_ptr<lockFile>> lockFiles;
std::mutex lockFilesMutex;

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
struct flock {
    short l_type;   // 锁的类型：F_RDLCK、F_WRLCK、F_UNLCK
    short l_whence; // 锁的起始位置：SEEK_SET、SEEK_CUR、SEEK_END
    off_t l_start;  // 锁定区域的起始位置
    off_t l_len;    // 锁定区域的长度
    pid_t l_pid;    // 锁定者的进程ID（可选）
};
/*lockHandle 实现以下函数更支持类unix系统的实现
void LockFile::lockHandle(int fd) {
    struct flock lock;
    lock.l_type = F_WRLCK; // 设置为写锁
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; // 锁定整个文件

    while (fcntl(fd, F_SETLKW, &lock) == -1) {
        if (errno == EINTR) {
            // 被信号中断时，重新尝试
            continue;
        }
        // 其他错误处理
        throw myerror("Failed to lock file: " + std::string(strerror(errno)));
    }
}*/
// lockHandle 实现windows
// 锁定函数
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
fileHandle openLock(const std::string& path, bool ro) {
    namespace fs = boost::filesystem;
    std::ios_base::openmode mode = fs::ofstream::out | fs::ofstream::binary;

    if (ro) {
        mode = fs::ifstream::in | fs::ifstream::binary;
    } else {
        mode |= fs::ifstream::in;
    }

    // 尝试打开文件
    try {
        // 创建 std::shared_ptr<boost::filesystem::ofstream> 实例
        auto fileStream = std::make_shared<fs::ofstream>(path, mode);
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
void lockFile::lock(LockType lType) {
    if (lType == LockType::ReadLock) {
        boost::unique_lock<boost::shared_mutex> lock(rwMutex, boost::defer_lock);
        lock.lock();  // 读锁
    } else {
        boost::unique_lock<boost::shared_mutex> lock(rwMutex);  // 写锁
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

void lockFile::Lock() {
    if (ro) {
        throw myerror("can't take write lock on read-only lock file");
    } else {
        lock(LockType::WriteLock);
    }
}

// 从数据创建 lastwrite 的函数
lastwrite newLastWriteFromData(const std::vector<uint8_t>& serialized) {
    // if (serialized.empty()) {
    //     throw std::invalid_argument("newLastWriteFromData with empty data");
    // }
    lastwrite lw;
    lw.state = serialized;
    return lw;
}
void lockFile::AssertLocked() {
    if (!locked) {
        throw myerror("internal error: lock is not held by the expected owner");
    }
}

// 实现 GetLastWrite 方法
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
std::vector<uint8_t> serializeLastWrite(const lastwrite& lw) {
    return lw.state;  // 示例实现，具体序列化方式应根据需求完成
}

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




void unlockAndCloseHandle(fileHandle fd) {
    if (fd && fd->is_open()) {
        fd->close();
    }
}

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

std::shared_ptr<lockFile> GetLockFile(const std::string& path) {
    return getLockfile(path, false);
}


std::pair<lastwrite, bool> lockFile::ModifiedSince(const lastwrite& previous) {
    AssertLocked(); // 确保当前已经持有锁
    lastwrite currentLW = GetLastWrite(); // 获取当前的 lastwrite 状态

    bool modified = !(previous.state == currentLW.state); // 检查状态是否改变
    return std::make_pair(currentLW, modified);
}
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