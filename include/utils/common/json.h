#if !defined(UTILS_COMMON_JSON_H)
#define UTILS_COMMON_JSON_H
#include <boost/json.hpp>
#include <iostream>
#include <string>
#include <vector>
namespace json = boost::json;
template <typename T>
std::string marshal(const T& obj);

template <typename T>
T unmarshal(const std::string& jsonStr);


#endif // UTILS_COMMON_JSON_H)