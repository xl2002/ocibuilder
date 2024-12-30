/**
 * @file args.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对参数列表的操作
 * <p>对参数表的操作函数的声明
 * @version 0.1
 * @date 2024-07-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COBRA_ARGS_H
#define COBRA_ARGS_H
#include <iostream>
#include <vector>
#include <functional>
#include "utils/cli/cobra/command.h"
#include "utils/common/error.h"
#include <string>
using std::vector;
using std::function;
using std::cerr;

function<bool(Command&,vector<string>&)> MaximumNArgs(int);
function<bool(Command&,vector<string>&)> MinimumNArgs(int n);

#endif