#if !defined(STORAGE_LOCKFILE_LOCKFILE_H)
#define STORAGE_LOCKFILE_LOCKFILE_H
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <mutex>
#include <chrono>
#include <atomic>
#include <fcntl.h>
//#include <unistd.h>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include "storage/lockfile/lastwrite.h"
#include "utils/common/error.h"
#include "storage/types/options.h"
using namespace std;

enum class LockType { ReadLock, WriteLock };
typedef std::shared_ptr<boost::filesystem::fstream> fileHandle;
class lockFile {
public:
    lockFile() = default;
    // The following fields are only set when constructing *LockFile, and must never be modified afterwards.
    // They are safe to access without any other locking.
    // 构造函数
    lockFile(const std::string& file, bool ro, LockType lockType)
        : file(file), ro(ro), lockType(lockType), counter(0), locked(false), fd(nullptr) {
        // 初始化其他成员变量
    }
    std::string file;
    bool ro=false;

    // rwMutex serializes concurrent reader-writer acquisitions in the same process space
    boost::shared_mutex rwMutex;
    // stateMutex is used to synchronize concurrent accesses to the state below
    boost::mutex stateMutex;
    std::atomic<int64_t> counter;
    lastwrite lw; // A global value valid as of the last .Touch() or .Modified()
    LockType lockType;
    std::atomic<bool> locked;
    // The following fields are only modified on transitions between counter == 0 / counter != 0.
    // Thus, they can be safely accessed by users _that currently hold the LockFile_ without locking.
    // In other cases, they need to be protected using stateMutex.
    fileHandle fd; // Note: fileHandle is replaced with int for simplicity
public:
    void Lock();
    void lock(LockType lType);
    void Unlock();
    lastwrite GetLastWrite();
    void AssertLocked();
    bool IsReadWrite() const {
        return !ro;
    }
    std::pair<lastwrite, bool> ModifiedSince(const lastwrite& previous);
    void AssertLockedForWriting();
    lastwrite RecordWrite();
};
std::shared_ptr<lockFile> GetLockFile(const std::string& path);
void unlockAndCloseHandle(fileHandle fd);
extern std::map<std::string, std::shared_ptr<lockFile>> lockFiles;
#endif