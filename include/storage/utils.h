/**
 * @file options.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief options.h 声明与可选项option相关的结构和方法
 * @version 0.1
 * @date 2024-07-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#if !defined(STIRAGE_UTILS_H)
#define STIRAGE_UTILS_H
#include <string>
#include <map>
#include <vector>
#include "cobra/error.h"
#include <cstdlib>   // For getenv
#include <sys/stat.h> // For stat
// #include <stdexcept> // For runtime_error
#include <fstream>   // For ifstream
#include <sstream>   // For stringstream
using std::string;
using std::map;
using std::vector;
/**
 * @brief 记录镜像存储方面的信息
 * 
 */
class StoreOptions{
    public:
    string GraphRoot;                       ///<
    string ImageStore;                      ///<
    string GraphDriverName;                 ///<
    vector<string> GraphDriverPriority;     ///<
    vector<string> GraphDriverOptions;      ///<
    map<string,string> PullOptions;         ///<
};
extern const string overlayDriver;
extern const string overlay2;
extern const string storageConfEnv;
//函数声明
StoreOptions DefaultStoreOptions();
bool usePerUserStorage();
std::string getConfigHome();
std::string getHomeDir();
// bool fileExists(const std::string& filename);
std::string DefaultConfigFile();
StoreOptions loadStoreOptionsFromConfFile(const std::string& storageConf);
StoreOptions loadStoreOptions();



#endif // MACRO
