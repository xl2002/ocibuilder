#if !defined(RUNTIME_RUNTIME2_H)
#define RUNTIME_RUNTIME2_H
#include <string>
#include "runtime/symtab.h"
#include "runtime/type.h"
using funcID = uint8_t;

using funcFlag = uint8_t;
struct _func {
    uint32_t entryOff;   // 从moduledata.text/pcHesader.textStart开始的偏移量
    int32_t nameOff;     // 函数名称，在moduledata.funcnametab中的索引

    int32_t args;        // 输入/输出参数大小
    uint32_t deferreturn;// 如果存在，从entry开始的deferreturn调用指令的偏移量

    uint32_t pcsp;
    uint32_t pcfile;
    uint32_t pcln;
    uint32_t npcdata;
    uint32_t cuOffset;   // 此函数CU的runtime.cutab偏移量
    int32_t startLine;   // 函数开始的行号（func关键字/TEXT指令）
    ::funcID funcID;       // 为某些特殊的runtime函数设置
    std::shared_ptr<funcFlag> flag=std::make_shared<funcFlag>();       // 函数标志
    uint8_t padding[1];  // 填充字节
    uint8_t nfuncdata;   // 必须是最后一个成员，并且必须在uint32_t对齐边界上结束

    // 结构体的末尾紧跟两个可变长度的数组，这些数组引用了此函数的pcdata和funcdata位置。

    // uint32_t pcdata[];   // npcdata大小的pcdata数组
    // uint32_t funcdata[]; // nfuncdata大小的funcdata数组
};

struct itab {
    std::shared_ptr<interfacetype> inter=std::make_shared<interfacetype>(); // 指向 interfacetype 的指针
    std::shared_ptr<::_type> _type=std::make_shared<::_type>();         // 指向 _type 的指针
    uint32_t hash;        // 拷贝自 _type.hash，用于类型切换
    uint8_t padding[4];   // 用于对齐的填充字节
    uintptr_t fun[1];     // 可变大小的数组，fun[0]==0 表示 _type 不实现 inter
};
#endif // RUNTIME_SPEC_RUNTIME2_H
