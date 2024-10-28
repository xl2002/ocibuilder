#if !defined(DOCKER_HELPERS_H)
#define DOCKER_HELPERS_H
#include <string>
#include <stdexcept>
#include <memory>
#include <regex>
#include "reference/reference.h"

bool IsNameOnly(const std::shared_ptr<Named_interface>& ref);

#endif