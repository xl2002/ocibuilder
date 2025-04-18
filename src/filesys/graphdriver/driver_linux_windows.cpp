#include "filesys/graphdriver/driver_linux_windows.h"

/**
 * @brief 获取指定路径的文件系统类型标识
 * @param rootpath 要检查的根路径
 * @return 返回文件系统魔法常量 
 * @throws myerror 当无法获取文件系统信息时抛出
 */
FsMagic GetFSMagic(const std::string& rootpath) {
    try {
        // 提取文件目录
        boost::filesystem::path path = boost::filesystem::absolute(rootpath).parent_path();

#ifdef __linux__
        
        // 类 Unix 系统 (Linux, macOS 等) 使用 statvfs 获取文件系统魔法常量
        struct statvfs buf;
        if (statvfs(path.c_str(), &buf) != 0) {
            // 若系统调用失败，抛出 myerror 类型错误
            throw myerror("无法获取文件系统信息: " + path.string());
        }
        // 返回文件系统魔法常量
        return static_cast<FsMagic>(buf.f_fsid);
#else
        // 在 Windows 平台上直接返回不支持的魔法常量
        return FsMagicUnsupported;
#endif
    } catch (const myerror& e) {
        // 捕获 myerror 类型错误并重新抛出
        throw;
    } catch (const std::exception& e) {
        // 捕获其他标准异常并转换为 myerror
        throw myerror("未知错误: " + std::string(e.what()));
    }
}
