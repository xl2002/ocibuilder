#if !defined(FILESYS_SYSTEM_SELINUX_SELINUX_LINUX_H)
#define FILESYS_SYSTEM_SELINUX_SELINUX_LINUX_H

#include <string>
#include <vector>
#include <mutex>
#include <map>
#include <memory>
#include <boost/thread/once.hpp>
class selinuxState {
public:
    // mcsList 用于存储 MCS（Multi-Category Security）的状态
    std::map<std::string, bool> mcsList;

    // selinuxfs 存储 SELinux 文件系统的路径
    std::string selinuxfs;

    // selinuxfsOnce 用于确保 selinuxfs 的初始化只执行一次
    boost::once_flag selinuxfsOnce;

    // enabledSet 标志是否已设置 enabled 状态
    bool enabledSet=false;

    // enabled 表示 SELinux 是否启用
    bool enabled=false;

    // 保护 shared data 的互斥锁
    std::mutex mtx;

    selinuxState()=default;
    bool setEnable(bool enabled);
    bool getEnabled();
    // bool setEnable(bool enabled);
};
bool getEnabled();
void setDisabled();

#endif // SELINUX_SELINUX_LINUX_H)
