#if !defined(STORAGE_LOCKFILE_LASTWRITE_H)
#define STORAGE_LOCKFILE_LASTWRITE_H
#include <vector>
#include <cstdint>
#include "utils/common/error.h"
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <atomic>
#include <chrono>
#include <cstring>
#include "filesys/systems.h"
#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;
class lastwrite {
public:
    // Contents of the lock file.
    std::vector<uint8_t> state;
public:
    lastwrite() = default;
    lastwrite(std::vector<uint8_t> s) : state(std::move(s)) {}
};
lastwrite newLastWrite();
extern std::atomic<uint64_t> lastWriterIDCounter;
extern size_t lastWriterIDSize;
#endif