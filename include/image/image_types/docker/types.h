#if !defined(IMAGE_IMAGE_TYPES_DOCKER_TYPES_H)
#define IMAGE_IMAGE_TYPES_DOCKER_TYPES_H
#include <string>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <chrono>
#include "utils/common/go/string.h"
#include "filesys/system/specs/specs.h"
#include <boost/optional.hpp>
#include "image/digest/digest.h"
class HealthConfig{
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

struct Port{
    std::string p;
    Port()=default;
    Port(std::string port):p(port){}
    std::string port(){
        return split(p,'/')[0];
    }
    std::string proto(){
        auto parts=split(p,'/');
        if(parts.size()==1){
            return "tcp";
        }
        return parts[1];
    }
    std::string String(){
        return p;
    }
};
struct PortSet{
    std::set<Port> ports;
};


class container_Config {
public:
    std::string Hostname;
    std::string Domainname;
    std::string User;
    int64_t Memory = 0;
    int64_t MemorySwap = 0;
    int64_t MemoryReservation = 0;
    int64_t KernelMemory = 0;
    int64_t CPUShares = 0;
    std::string CPUSet;
    std::vector<std::string> PortSpecs;
    std::vector<Port> ExposedPorts; // 这里将Port类型替换为std::string
    std::string PublishService;
    std::string StopSignal;
    int StopTimeout = 0;
    std::vector<std::string> Env;
    std::vector<std::string> Cmd;
    std::vector<std::string> Shell;
    std::shared_ptr<HealthConfig> Healthcheck = std::make_shared<HealthConfig>(); // 使用指针以处理可选的HealthConfig
    std::vector<std::string> DNS;
    std::string Image;
    std::map<std::string, std::string> Volumes; // 这里也将struct{}替换为std::string
    std::string VolumeDriver;
    std::string WorkingDir;
    std::string MacAddress;
    std::vector<std::string> Entrypoint;
    std::vector<std::string> SecurityOpts;
    std::vector<std::string> OnBuild;
    std::vector<Mount> Mounts;
    std::map<std::string, std::string> Labels;
    bool AttachStdin = false;
    bool AttachStdout = false;
    bool AttachStderr = false;
    bool ArgsEscaped = false;
    bool Tty = false;
    bool OpenStdin = false;
    bool StdinOnce = false;
    bool NetworkDisabled = false;
    std::string VolumesFrom;
    container_Config() = default;
};

struct V1Image {
    // ID 是镜像的唯一 64 字符标识符
    std::string ID;
    
    // Parent 是父镜像的 ID
    std::string Parent;
    
    // Comment 是提交镜像时设置的提交消息
    std::string Comment;
    
    // Created 是镜像创建的时间戳
    std::chrono::system_clock::time_point Created;  // 使用 time_t 表示时间
    
    // Container 是用于提交的容器的 ID
    std::string Container;
    
    // ContainerConfig 是提交到镜像中的容器配置
    std::shared_ptr<container_Config> ContainerConfig=std::make_shared<container_Config>();
    
    // DockerVersion 指定用于构建镜像的 Docker 版本
    std::string DockerVersion;
    
    // Author 是提交镜像时指定的作者名称
    std::string Author;
    
    // Config 是从客户端收到的容器配置
    std::shared_ptr<container_Config> config=std::make_shared<container_Config>();  // 使用 boost::optional 来表示可选字段
    
    // Architecture 是镜像构建和运行的硬件架构
    std::string Architecture;
    
    // Variant 是镜像构建和运行的 CPU 变体
    std::string Variant;
    
    // OS 是用于构建和运行镜像的操作系统
    std::string OS;
    
    // Size 是包括镜像所有层的总大小
    int64_t Size=0;  // 使用 boost::optional 来表示可选字段
};
struct ID{
    std::shared_ptr<::Digest> digest=std::make_shared<::Digest>();
    ~ID()=default;
    ID()=default;
};
struct V2S2RootFS{
    std::string Type;
    std::vector<std::shared_ptr<Digest>> DiffIDs;
    V2S2RootFS()=default;
};
struct V2S2History{
    std::chrono::system_clock::time_point Created;
    std::string Author;
    std::string CreatedBy;
    std::string Comment;
    bool EmptyLayer=false;
    V2S2History()=default;
};
struct V2Image:public V1Image{
    std::shared_ptr<::ID> Parent =std::make_shared<::ID>();
    std::shared_ptr<V2S2RootFS> RootFS=std::make_shared<V2S2RootFS>();
    std::shared_ptr<V2S2History>History=std::make_shared<V2S2History>();
    std::string OSVersion;
    std::vector<std::string> OSFeatures;
    V2Image()=default;
};
#endif // DOCKER_TYPES_H)