#include "filesys/system/selinux/selinux_linux.h"


const int minSensLen       = 2;
const std::string
	contextFile      = "/usr/share/containers/selinux/contexts",
	selinuxDir       = "/etc/selinux/",
	selinuxUsersDir  = "contexts/users",
	defaultContexts  = "contexts/default_contexts",
	selinuxConfig    = selinuxDir + "config",
	selinuxfsMount   = "/sys/fs/selinux",
	selinuxTypeTag   = "SELINUXTYPE",
	selinuxTag       = "SELINUX",
	xattrNameSelinux = "security.selinux";

// 全局变量
// std::string readOnlyFileLabel;

// auto state=std::make_shared<selinuxState>();

// for attrPath()
boost::once_flag attrPathOnce;
// bool haveThreadSelf = false;

// for policyRoot()
boost::once_flag policyRootOnce;
// std::string policyRootVal;

// for label()
boost::once_flag loadLabelsOnce;
// std::map<std::string, std::string> labels;

// 设置 enabled 状态，并返回设置后的状态
/**
 * @brief Set SELinux enabled state
 * 
 * @param enabled Whether to enable SELinux
 * @return bool Current enabled state
 */
bool selinuxState::setEnable(bool enabled) {
    std::lock_guard<std::mutex> lock(mtx);  // 加锁，作用域结束时自动解锁
    enabledSet = true;
    this->enabled = enabled;
    return this->enabled;
}
/**
 * @brief Get current SELinux enabled state
 * 
 * @return bool True if SELinux is enabled, false otherwise
 */
bool selinuxState::getEnabled() {
    // 锁定互斥锁，保证线程安全
    mtx.lock();
    bool localEnabled = enabled;
    bool localEnabledSet = enabledSet;
    mtx.unlock();

    // 如果 enabledSet 已经设置，则返回 enabled 的值
    if (localEnabledSet) {
        return localEnabled;
    }

    // 默认设置为 false
    localEnabled = false;

    // // 检查 SELinux 挂载点和当前标签
    // std::string fs = getSelinuxMountPoint();
    // if (!fs.empty()) {
    //     std::string con = CurrentLabel();
    //     if (con != "kernel") {
    //         localEnabled = true;
    //     }
    // }

    // 使用 setEnable 方法设置并返回 enabled 的值
    return setEnable(localEnabled);
}
// bool selinuxState::setEnable(bool en) {
//     std::lock_guard<std::mutex> lock(mtx);  // 加锁，作用域结束时自动解锁
//     enabled=en;
//     enabledSet=true;
//     return enabled;
// }
/**
 * @brief Global function to get SELinux enabled state
 * 
 * @return bool True if SELinux is enabled, false otherwise
 */
bool getEnabled() {
    return state->getEnabled();
}
/**
 * @brief Global function to disable SELinux
 */
void setDisabled(){
    state->setEnable(false);
}
