#if !defined(GO_TIME_H)
#define GO_TIME_H
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <time.h>
std::string time_point_to_string(const std::chrono::system_clock::time_point& tp);

#endif // GO_TIME_H)
