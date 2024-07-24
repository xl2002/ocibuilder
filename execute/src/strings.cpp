#include "src/strings.h"


bool HasPrefix(const std::string& data, const std::string& prefix)
{
    return data.size() >= prefix.size() && data.compare(0, prefix.size(), prefix) == 0;
}