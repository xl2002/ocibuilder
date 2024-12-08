#if !defined(STORAGE_STORAGE_STRINGID_H)
#define STORAGE_STORAGE_STRINGID_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include "utils/common/error.h"
#include "utils/common/regexp.h"
#include <iostream>
// #include <vector>
// #include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <cctype>
std::string GenerateRandomID();
std::string encodeToHex(const std::vector<uint8_t>& bytes);
std::string truncateID(const std::string& id);
std::string generateID();
#endif // STORAGE_STORAGE_STRINGID_H)
