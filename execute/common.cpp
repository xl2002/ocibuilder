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
    flags.BoolVar(br.allplatform,"all-platforms",false,"attempt to build for all base image platforms");
    flags.String("arch","amd64","set the ARCH of the image to the provided value instead of the architecture of the host");
    flags.StringArrayVar(br.annotation,"annotation",vector<string>(),"set metadata for an image (default [])");
    flags.StringArrayVar(br.tag,"tag",vector<string>(),"tagged `name` to apply to the built image");
    flags.StringVar(br.osversion,"os-version",string(),"set required OS `version` for the target image instead of the value from the base image");
    // flags.StringArrayVar(br.annotation,"",,"");
    // flags.String();
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
    // flags.BoolVar();
    return flags;
}