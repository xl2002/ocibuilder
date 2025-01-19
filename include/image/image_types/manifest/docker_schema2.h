#if !defined(IMAGE_IMAGE_TYPES_MANIFEST_DOCKER_SCHEMA_H)
#define IMAGE_IMAGE_TYPES_MANIFEST_DOCKER_SCHEMA_H
#include <string>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <chrono>
class Schema2Port{
    public:
    std::string schema2port;
};

class Schema2PortSet{
    public:
    std::set<Schema2Port> ports;
};
class Schema2HealthConfig{
    public:
    // Test is the test to perform to check that the container is healthy.
    // An empty vector means to inherit the default.
    // The options are:
    // {} : inherit healthcheck
    // {"NONE"} : disable healthcheck
    // {"CMD", args...} : exec arguments directly
    // {"CMD-SHELL", command} : run command with system's default shell
    std::vector<std::string> Test;

    // Zero means to inherit. Durations are expressed as integer nanoseconds.
    std::chrono::nanoseconds Interval{0};    // Interval is the time to wait between checks.
    std::chrono::nanoseconds Timeout{0};     // Timeout is the time to wait before considering the check to have hung.
    std::chrono::nanoseconds StartPeriod{0}; // The start period for the container to initialize before the retries starts to count down.

    // Retries is the number of consecutive failures needed to consider a container as unhealthy.
    // Zero means inherit.
    int Retries=0;

};

class Schema2Config{
    public:
    std::string Hostname;               // 主机名
    std::string Domainname;             // 域名
    std::string User;                   // 容器中运行命令的用户，支持 user:group
    bool AttachStdin=false;                   // 是否附加标准输入
    bool AttachStdout=false;                  // 是否附加标准输出
    bool AttachStderr=false;                  // 是否附加标准错误
    std::shared_ptr<Schema2PortSet> ExposedPorts=std::make_shared<Schema2PortSet>();        // 暴露的端口列表
    bool Tty=false;                           // 附加到 TTY
    bool OpenStdin=false;                     // 打开标准输入
    bool StdinOnce=false;                     // 如果为真，连接的客户端断开后关闭标准输入
    std::vector<std::string> Env;       // 容器内设置的环境变量列表
    std::vector<std::string> Cmd;                       // 启动容器时运行的命令
    std::shared_ptr<Schema2HealthConfig> Healthcheck=std::make_shared<Schema2HealthConfig>(); // 容器的健康检查配置
    bool ArgsEscaped=false;                   // 如果命令已经被转义，设置为真 (特定于 Windows)
    std::string Image;                  // 镜像名称
    std::set<std::string> Volumes; // 容器使用的卷列表
    std::string WorkingDir;             // 启动命令的当前目录
    std::vector<std::string> Entrypoint;                // 启动容器时运行的入口点
    bool NetworkDisabled=false;               // 是否禁用网络
    std::string MacAddress;             // 容器的 MAC 地址
    std::vector<std::string> OnBuild;   // 镜像 Dockerfile 中定义的 ONBUILD 元数据
    std::map<std::string, std::string> Labels; // 容器设置的标签列表
    std::string StopSignal;             // 停止容器时的信号
    std::shared_ptr<int> StopTimeout=std::make_shared<int>(0);   // 停止容器的超时时间（秒）
    std::vector<std::string> Shell;                     // RUN、CMD、ENTRYPOINT 的 shell 格式
};
#endif // MANIFEST_DOCKER_SCHEMA_H
