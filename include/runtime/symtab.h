#if !defined(RUNTIME_SYMTAB_H)
#define RUNTIME_SYMTAB_H
#include <string>
#include <cstdint>
#include <iostream>
#include <vector>
#include <map>
#include <memory>   
#include "runtime/runtime2.h"
#include "runtime/type.h"
struct _func;
struct itab;
class Func {
public:
    // 私有成员，unexported 字段
    struct Opaque {
        // 这里可以定义一些私有的数据或结构体
    } opaque;

    std::string funcName;

public:
    // 构造函数
    Func() = default;
    Func(const std::string& name) : funcName(name) {}

    // 公有接口，用于获取函数名称
    std::string Name() const {
        return funcName;
    }

    // 获取内部的 Opaque 对象
    std::shared_ptr<Opaque> GetOpaque() {
        return std::make_shared<Opaque>(opaque);
    }

};


// using funcID = uint8_t;

// using funcFlag = uint8_t;


// pcHeader 结构体用于存储与 pclntab 查找相关的数据
// 可能需要注意对齐和大小一致性
struct pcHeader {
    uint32_t magic;           // 0xFFFFFFF1
    uint8_t pad1;             // 填充字节
    uint8_t pad2;             // 填充字节
    uint8_t minLC;            // 最小指令大小
    uint8_t ptrSize;          // 指针大小（字节数）
    int nfunc;                // 模块中的函数数量
    uint32_t nfiles;          // 文件表中的条目数量
    uintptr_t textStart;      // 函数入口 PC 偏移的基地址，等于 moduledata.text
    uintptr_t funcnameOffset; // 从 pcHeader 到 funcnametab 变量的偏移量
    uintptr_t cuOffset;       // 从 pcHeader 到 cutab 变量的偏移量
    uintptr_t filetabOffset;  // 从 pcHeader 到 filetab 变量的偏移量
    uintptr_t pctabOffset;    // 从 pcHeader 到 pctab 变量的偏移量
    uintptr_t pclnOffset;     // 从 pcHeader 到 pclntab 变量的偏移量

    // 构造函数用于初始化字段
    pcHeader()
        : magic(0xFFFFFFF1), pad1(0), pad2(0), minLC(0), ptrSize(0),
            nfunc(0), nfiles(0), textStart(0), funcnameOffset(0),
            cuOffset(0), filetabOffset(0), pctabOffset(0), pclnOffset(0) {}
};


struct functab {
    uint32_t entryoff;  // 相对于runtime.text的偏移量
    uint32_t funcoff;
};

struct textsect {
    uintptr_t vaddr;     // 预链接的虚拟地址
    uintptr_t end;       // vaddr + section length
    uintptr_t baseaddr;  // 重定位段地址
};

// 编译器为插件主包中的每个导出函数和全局变量生成一个ptabEntry。
// 它用于初始化插件模块的符号映射。
struct ptabEntry {
    nameOff name;     // 函数名
    typeOff type;     // 函数类型
};

struct modulehash{
    std::string modulename;
    std::string linktimehash;
    std::shared_ptr<std::string> runtimehash=std::make_shared<std::string>();
};

struct bitvector{
    uint32_t n;
    std::shared_ptr<uint8_t> bytedata=std::make_shared<uint8_t>(0);
};


struct moduledata {
    std::shared_ptr<pcHeader> pcHeaderPtr=std::make_shared<pcHeader>();                        // 指向 pcHeader 的指针
    std::vector<uint8_t> funcnametab;             
    std::vector<uint32_t> cutab;                  
    std::vector<uint8_t> filetab;                 
    std::vector<uint8_t> pctab;                   
    std::vector<uint8_t> pclntable;               
    std::vector<functab> ftab;                    
    uintptr_t findfunctab;                        
    uintptr_t minpc, maxpc;                       

    uintptr_t text, etext;                        
    uintptr_t noptrdata, enoptrdata;              
    uintptr_t data, edata;                        
    uintptr_t bss, ebss;                          
    uintptr_t noptrbss, enoptrbss;                
    uintptr_t covctrs, ecovctrs;                  
    uintptr_t end, gcdata, gcbss;                 
    uintptr_t types, etypes;                      
    uintptr_t rodata;                             
    uintptr_t gofunc;                             

    std::vector<textsect> textsectmap;            
    std::vector<int32_t> typelinks;               // 从类型的偏移量
    std::shared_ptr<std::vector<itab>> itablinks=std::make_shared<std::vector<itab>>();                 

    std::vector<ptabEntry> ptab;                  

    std::string pluginpath;                       
    std::vector<modulehash> pkghashes;            

    std::string modulename;                       
    std::vector<modulehash> modulehashes;         

    uint8_t hasmain;                              

    bitvector gcdatamask, gcbssmask;              

    std::shared_ptr<std::map<int32_t, _type>> typemap=std::make_shared<std::map<int32_t, _type>>();            

    bool bad;                                     

    std::shared_ptr<moduledata> next=std::make_shared<moduledata>();                             // 指向下一个 moduledata
};

// funcInfo 结构体
struct funcInfo {
    std::shared_ptr<_func> funcPtr=std::make_shared<_func>();        // 指向 _func 的指针
    std::shared_ptr<moduledata> datap=std::make_shared<moduledata>();     // 指向 moduledata 的指针
};


// Frame 结构体
struct Frame {
    
    uintptr_t PC;        // 程序计数器（PC）用于表示当前帧的位置
    std::shared_ptr<Func> funcPtr=std::make_shared<Func>();          // 函数指针，用于表示当前帧所对应的函数   
    std::string Function;// 函数名，包路径限定的函数名

    std::string File;    // 文件名和行号，表示当前帧的位置
    int Line;

    int startLine;       // 函数开始的行号
    uintptr_t Entry;     // 函数的入口点程序计数器
    funcInfo funcInfo;   // 函数的内部视图，仅对 Go 函数有效


    Frame(uintptr_t pc = 0, std::shared_ptr<Func> func = nullptr, std::string function = "",
            std::string file = "", int line = 0, int startLine = 0,
            uintptr_t entry = 0)
        : PC(pc), funcPtr(func), Function(std::move(function)),
            File(std::move(file)), Line(line), startLine(startLine),
            Entry(entry) {}
};

#endif // RUNTIME_SPEC_SYMTAB_H
