#if !defined(DOCKER_CONTAINER_H)
#define DOCKER_CONTAINER_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include "go/string.h"
#include "specs/specs.h"
// // #include 
// class HealthConfig{
//     public:
//     // Test is the test to perform to check that the container is healthy.
//     // An empty vector means to inherit the default.
//     // The options are:
//     // {} : inherit healthcheck
//     // {"NONE"} : disable healthcheck
//     // {"CMD", args...} : exec arguments directly
//     // {"CMD-SHELL", command} : run command with system's default shell
//     std::vector<std::string> Test;

//     // Zero means to inherit. Durations are expressed as integer nanoseconds.
//     std::chrono::nanoseconds Interval{0};    // Interval is the time to wait between checks.
//     std::chrono::nanoseconds Timeout{0};     // Timeout is the time to wait before considering the check to have hung.
//     std::chrono::nanoseconds StartPeriod{0}; // The start period for the container to initialize before the retries starts to count down.

//     // Retries is the number of consecutive failures needed to consider a container as unhealthy.
//     // Zero means inherit.
//     int Retries=0;

// };

// struct Port{
//     std::string p;
//     Port()=default;
//     Port(std::string port):p(port){}
//     std::string port(){
//         return split(p,'/')[0];
//     }
//     std::string proto(){
//         auto parts=split(p,'/');
//         if(parts.size()==1){
//             return "tcp";
//         }
//         return parts[1];
//     }
// };
// class container_Config {
// public:
//     std::string Hostname;
//     std::string Domainname;
//     std::string User;
//     int64_t Memory = 0;
//     int64_t MemorySwap = 0;
//     int64_t MemoryReservation = 0;
//     int64_t KernelMemory = 0;
//     int64_t CPUShares = 0;
//     std::string CPUSet;
//     std::vector<std::string> PortSpecs;
//     std::map<std::string, Port> ExposedPorts; // 这里将Port类型替换为std::string
//     std::string PublishService;
//     std::string StopSignal;
//     int StopTimeout = 0;
//     std::vector<std::string> Env;
//     std::vector<std::string> Cmd;
//     std::vector<std::string> Shell;
//     std::shared_ptr<HealthConfig> Healthcheck = nullptr; // 使用指针以处理可选的HealthConfig
//     std::vector<std::string> DNS;
//     std::string Image;
//     std::map<std::string, std::string> Volumes; // 这里也将struct{}替换为std::string
//     std::string VolumeDriver;
//     std::string WorkingDir;
//     std::string MacAddress;
//     std::vector<std::string> Entrypoint;
//     std::vector<std::string> SecurityOpts;
//     std::vector<std::string> OnBuild;
//     std::vector<Mount> Mounts;
//     std::map<std::string, std::string> Labels;
//     bool AttachStdin = false;
//     bool AttachStdout = false;
//     bool AttachStderr = false;
//     bool ArgsEscaped = false;
//     bool Tty = false;
//     bool OpenStdin = false;
//     bool StdinOnce = false;
//     bool NetworkDisabled = false;
//     std::string VolumesFrom;
// };

#endif // DOCKER_CONTAINER_H
