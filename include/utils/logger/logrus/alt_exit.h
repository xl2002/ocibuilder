#if !defined(UTILS_LOGGER_LOGRUS_ALT_EXIT_H)
#define UTILS_LOGGER_LOGRUS_ALT_EXIT_H
#include <iostream>
#include <vector>
#include <functional>
#include <exception>
#include "utils/common/error.h"
void runHandler(const std::function<void()>& handler);
void runHandlers();
#endif // LOGRUS_ALT_EXIT_H