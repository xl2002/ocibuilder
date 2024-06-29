/**
 * @file common.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对common.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "common.h"
/**
 * @brief 返回通用构建标志
 * 
 * @param br 
 * @return Flagset 
 */
Flagset Getbuildflags(BuildOptions& br){
    Flagset flags;
    flags.StringVar();
    flags.StringArrayVar();
    flags.String();
    return flags;
}

/**
 * @brief 返回图层的通用标志
 * 
 * @param lr 
 * @return Flagset 
 */
Flagset GetLayerFlags(LayerOptions& lr){
    Flagset flags;
    flags.BoolVar();
    return flags;
}