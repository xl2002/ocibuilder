#if !defined(UTILS_LOGGER_LOGRUS_ALT_EXIT_H)
#define UTILS_LOGGER_LOGRUS_ALT_EXIT_H
#include <iostream>
#include <vector>
#include <functional>
#include <exception>
#include "utils/common/error.h"
// 定义 handlers 为一个存储函数的 vector
extern std::vector<std::function<void()>> handlers;
void runHandler(const std::function<void()>& handler);
void runHandlers();
#endif // LOGRUS_ALT_EXIT_H