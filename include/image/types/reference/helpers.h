#if !defined(IMAGE_TYPES_REFERENCE_HELPERS_H)
#define IMAGE_TYPES_REFERENCE_HELPERS_H
#include <string>
#include <stdexcept>
#include <memory>
#include <regex>
#include "image/types/reference/reference.h"

bool IsNameOnly(const std::shared_ptr<Named_interface>& ref);

#endif