#if !defined(RUNTIME_TYPE_H)
#define RUNTIME_TYPE_H
#include <string>
#include <vector>

typedef int32_t nameOff;
typedef int32_t typeOff;
typedef int32_t textOff;

using tflag = uint8_t;
using byte = uint8_t;
using Pointer = void*;
struct _type {
    std::uintptr_t size;         
    std::uintptr_t ptrdata;      
    std::uint32_t hash;          
    ::tflag tflag;                 
    std::uint8_t align;          
    std::uint8_t fieldAlign;     
    std::uint8_t kind;           

    // 函数指针，用于比较对象是否相等
    bool (*equal)(Pointer, Pointer); //  Go : func(unsafe.Pointer, unsafe.Pointer) bool

    // gcdata 用于存储垃圾回收器的类型数据
    byte* gcdata;                

    nameOff str;                 
    typeOff ptrToThis;           
};

struct imethod {
    nameOff name;
    typeOff type;
};
struct name {
    byte* bytes;  
};
struct interfacetype {
    _type typ;
    name pkgpath;
    std::vector<imethod> mhdr;
};

#endif // RUNTIME_SPEC_TYPE_H
