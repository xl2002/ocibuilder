#if !defined(UTILS_INIT_INIT_GLOBAL_H)
#define UTILS_INIT_INIT_GLOBAL_H
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
void init_cmd();
void init_config();
void init_filesys();
void init_image();
void init_network();
void init_security();
void init_storage();
void init_utils();
void initialize_global();

#endif // INIT_GLOBAL_H
