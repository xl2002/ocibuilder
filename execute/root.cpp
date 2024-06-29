/**
 * @file root.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 root.h头文件中定义的方法进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "root.h"
/**
 * @brief rootcmd的初始化函数
 * 
 * @return Command 返回rootcmd结构
 */
Command init_rootcmd(){
    Command Appcmd;
    Appcmd.name="buildah";
    Appcmd.Long="A tool that facilitates building OCI images";
    Appcmd.version="";
    // Appcmd.PersistentFlags().StringVar
    return Appcmd;
}


Command rootcmd=init_rootcmd(); ///<定义rootcmd，作为全局变量