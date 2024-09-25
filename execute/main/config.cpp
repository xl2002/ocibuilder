/**
 * @file config.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 config.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "main/config.h"
// #include "command.h"

/**
 * @brief 初始化 config 命令的内容
 *
 * 这个函数用于初始化 config 命令的内容，它创建了一个 config 命令对象，并将其添加到根命令中。
 * 该命令对象具有以下属性：
 * - 命令名称为 "config"
 * - 简短介绍为 "Update image configuration settings"
 * - 详细介绍为 "Modifies the configuration values which will be saved to the image."
 * - 使用示例如下：
 *   - buildah config --author='Jane Austen' --workingdir='/etc/mycontainers' containerID
 *   - buildah config --entrypoint '[ \"/entrypoint.sh\", \"dev\" ]' containerID
 *   - buildah config --env foo=bar --env PATH=$PATH containerID
 *
 * 函数内部不执行 config 命令的 Run 操作，因此需要在其他地方定义 configCmd 函数。
 *
 * @return void
 */
typedef struct ConfigResults {
    bool addHistory;                          ///< Whether to add this operation to the image's history
    std::vector<std::string> annotation;     ///< Annotations for the image
    std::string arch;                         ///< Architecture of the target image
    std::string author;                       ///< Author's contact information
    std::string cmd;                          ///< Default command for containers based on the image
    std::string comment;                       ///< Comment for the target image
    std::string createdBy;                    ///< Description of how the image was created
    std::string domainName;                   ///< Domain name for containers based on the image
    std::string entrypoint;                   ///< Entrypoint for containers based on the image
    std::vector<std::string> env;             ///< Environment variables for the container
    std::string healthcheck;                  ///< Healthcheck command for the target image
    std::string healthcheckInterval;          ///< Interval for healthcheck command
    int healthcheckRetries;                   ///< Number of retries for healthcheck command on failure
    std::string healthcheckStartPeriod;      ///< Time to wait before healthcheck after container start
    std::string healthcheckTimeout;           ///< Timeout for the healthcheck command
    std::string historyComment;                ///< Comment for the image's history
    std::string hostname;                     ///< Hostname for containers based on the image
    std::vector<std::string> label;           ///< Configuration labels for the image
    std::vector<std::string> onbuild;         ///< ONBUILD commands for the image
    std::string os;                           ///< Operating system for the target image
    std::vector<std::string> osfeature;       ///< Required OS features for the target image
    std::string osversion;                    ///< Required OS version for the target image
    std::vector<std::string> ports;           ///< Exposed ports for the container
    std::string shell;                        ///< Shell to run in the container
    std::string stopSignal;                   ///< Stop signal for containers based on the image
    std::string user;                         ///< Default user for the container
    std::string variant;                      ///< Variant of the target image's architecture
    std::vector<std::string> volume;          ///< Default volume paths for the container
    std::string workingDir;                   ///< Working directory for containers based on the image
    std::vector<std::string> unsetLabels;     ///< Labels to remove from the image
}configResults;
string UsageTemplate(){
    string str{"Usage:{{if .Runnable}}\
                {{.UseLine}}{{end}}{{if .HasAvailableSubCommands}}\
                {{.CommandPath}} [command]{{end}}{{if gt (len .Aliases) 0}}\
                Aliases:\
                {{.NameAndAliases}}{{end}}{{if .HasExample}}\
                Examples:\
                {{.Example}}{{end}}{{if .HasAvailableSubCommands}}\
                Available Commands:{{range .Commands}}{{if (or .IsAvailableCommand (eq .Name \"help\"))}}\
                {{rpad .Name .NamePadding }} {{.Short}}{{end}}{{end}}{{end}}{{if .HasAvailableLocalFlags}}\
                Flags:\
                {{.LocalFlags.FlagUsages | trimTrailingWhitespaces}}{{end}}{{if .HasAvailableInheritedFlags}}\
                {{end}}"};
    return str;
}
void initConfigCommand() {
    // 声明配置描述和选项
    std::string configDescription = "\n  Modifies the configuration values which will be saved to the image.";
    configResults opts;

    std::string config_name = "config";
    std::string config_Short = "Update image configuration settings";
    std::string config_example = "Examples:\n"
                                  "  buildah config --author='Jane Austen' --workingdir='/etc/mycontainers' containerID\n"
                                  "  buildah config --entrypoint '[ \"/entrypoint.sh\", \"dev\" ]' containerID\n"
                                  "  buildah config --env foo=bar --env PATH=$PATH containerID";

    Command* configCommand = new Command(config_name, config_Short, configDescription, config_example);
    
    // 定义使用模板
    std::string Template = UsageTemplate();
    configCommand->SetUsageTemplate(Template);

    Flagset* flags = configCommand->Flags();
    flags->SetInterspersed(false);

    // 使用新的接口方法来添加标志
    flags->BoolVar(opts.addHistory, "add-history", false, "Add an entry for this operation to the image's history. Use BUILDAH_HISTORY environment variable to override. (default false)");
    flags->StringArrayVar(opts.annotation, "annotation", {}, "Add `annotation` e.g. annotation=value, for the target image (default [])");
    flags->StringVar(opts.arch, "arch", "amd64", "Set `architecture` of the target image to the provided value instead of the architecture of the host");
    flags->StringVar(opts.author, "author", "", "Set image author contact `information`");
    flags->StringVar(opts.cmd, "cmd", "", "Set the default `command` to run for containers based on the image");
    flags->StringVar(opts.comment, "comment", "", "Set a `comment` in the target image");
    flags->StringVar(opts.createdBy, "created-by", "", "Set `description` of how the image was created");
    flags->StringVar(opts.domainName, "domainname", "", "Set a domain `name` for containers based on image");
    flags->StringVar(opts.entrypoint, "entrypoint", "", "Set `entry point` for containers based on image");
    flags->StringArrayVar(opts.env, "env", {}, "Add `environment variable` to be set when running containers based on image (default [])");
    flags->StringVar(opts.healthcheck, "healthcheck", "", "Set a `healthcheck` command for the target image");
    flags->StringVar(opts.healthcheckInterval, "healthcheck-interval", "", "Set the `interval` between runs of the `healthcheck` command for the target image");
    flags->IntVar(opts.healthcheckRetries, "healthcheck-retries", 0, "Set the `number` of times the `healthcheck` command has to fail");
    flags->StringVar(opts.healthcheckStartPeriod, "healthcheck-start-period", "", "Set the amount of `time` to wait after starting a container before a failed `healthcheck` command will count as a failure");
    flags->StringVar(opts.healthcheckTimeout, "healthcheck-timeout", "", "Set the maximum amount of `time` to wait for a `healthcheck` command for the target image");
    flags->StringVar(opts.historyComment, "history-comment", "", "Set a `comment` for the history of the target image");
    flags->StringVar(opts.hostname, "hostname", "", "Set a host `name` for containers based on image");
    flags->StringArrayVar(opts.label, "label", {}, "Add image configuration `label` e.g. label=value");
    flags->StringArrayVar(opts.onbuild, "onbuild", {}, "Add onbuild command to be run on images based on this image. Only supported on 'docker' formatted images");
    flags->StringVar(opts.os, "os", "", "Set `operating system` of the target image");
    flags->StringArrayVar(opts.osfeature, "os-feature", {}, "Set required OS `feature` for the target image");
    flags->StringVar(opts.osversion, "os-version", "", "Set required OS `version` for the target image");
    flags->StringArrayVar(opts.ports, "port", {}, "Add `port` to expose when running containers based on image (default [])");
    flags->StringVar(opts.shell, "shell", "", "Add `shell` to run in containers");
    flags->StringVar(opts.stopSignal, "stop-signal", "", "Set `stop signal` for containers based on image");
    flags->StringVar(opts.user, "user", "", "Set default `user` to run inside containers based on image");
    flags->StringVar(opts.variant, "variant", "", "Set architecture `variant` of the target image");
    flags->StringArrayVar(opts.volume, "volume", {}, "Add default `volume` path to be created for containers based on image (default [])");
    flags->StringVar(opts.workingDir, "workingdir", "", "Set working `directory` for containers based on image");
    flags->StringArrayVar(opts.unsetLabels, "unsetlabel", {}, "Remove image configuration label");

    // 设置 Run 函数
    configCommand->Run = [&](Command& cmd, std::vector<std::string> args) {
        configCmd(cmd, args, opts);
    };

    rootcmd.AddCommand({configCommand});
}
/**
 * @brief config命令Run操作的
 * 
 */
void configCmd(Command& cmd, std::vector<std::string> args, configResults& iopts) {
    try {
        if (args.empty()) {
            throw myerror("Container ID must be specified.");
        }
        if (args.size() > 1) {
            throw myerror("Too many arguments specified.");
        }
        std::string name = args[0];

        // 获取存储对象
        auto store = getStore(&cmd);
        if (!store) {
            throw myerror("Failed to get store.");
        }

        // 打开构建器
        Builder builder = openBuilder(store, name);
        if (!builder) {
            throw myerror("Reading build container \"" + name + "\" failed.");
        }

        // 更新配置
        if (updateConfig(builder, cmd, iopts)) {
            throw myerror("Failed to update configuration.");
        }

        // 保存构建器
        if (builder->Save()) {
            throw myerror("Failed to save builder.");
        }

    } catch (const myerror& e) {
        // 只处理 myerror 类型的错误
        throw;
    }
}



