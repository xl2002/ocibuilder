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
#if !defined(OPTIONS_H)
#define OPTIONS_H
#include <string>
#include <map>
#include <vector>
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

StoreOptions DefaultStoreOptions();


#endif // MACRO
