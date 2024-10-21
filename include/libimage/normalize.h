#if !defined(LIBIMAGE_NORMALIZED_H)
#define LIBIMAGE_NORMALIZED_H

#include <string>
#include <vector>
#include <memory>
#include "reference/reference.h"
#include <sstream>
std::shared_ptr<named> NormalizeName(std::string name);
std::tuple<std::string, std::shared_ptr<Named_interface>> normalizeTaggedDigestedString(std::string s);
namespace libimage {
    std::tuple<std::string,std::string,std::string> Normalize(std::string rawOS,std::string rawArch,std::string rawVariant);
}
std::string ToString(const std::string& os, const std::string& arch, const std::string& variant);
#endif // LIBIMAGE_NORMALIZED_H
