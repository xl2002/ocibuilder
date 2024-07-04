/**
 * @file build.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 build.h头文件中定义的方法进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "build.h"


/**
 * @brief 初始化build命令的内容
 * 
 * 对build命令的内容进行初始化，包括命令名，命令介绍，命令用法介绍，build命令合法的标志集
 */
void init_buildcmd(){
    BuildOptions br;
    string build_name="build [context]";
    string build_Short="Build an image using instructions in a Containerfile";
    string build_Long={"Builds an OCI image using instructions in one or more Containerfiles.\n\
                If no arguments are specified, Buildah will use the current working directory\n\
                as the build context and look for a Containerfile. The build fails if no\n\
                Containerfile nor Dockerfile is present."};
    string build_example={"buildah build\n\
                    buildah bud -f Containerfile.simple.\n\
                    buildah bud --volume /home/test:/myvol:ro,Z -t imageName.\n\
                    buildah bud -f Containerfile.simple -f Containerfile.notsosimple."};
    Command build_Command{build_name,build_Short,build_Long,build_example};
    string Template=UsageTemplate();
    build_Command.SetUsageTemplate(Template);
    build_Command.Args=MaximumNArgs(1);

    Flagset& flags=build_Command.Flags();
    flags.SetInterspersed(false);
    
    Flagset buildflags=Getbuildflags(br);
    flags.AddFlagSet(buildflags);
    build_Command.Run=buildCmd;
    rootcmd.AddCommand({build_Command});
    build_Command.Run();
    // return build_Command;
}

/**
 * @brief build 命令Run操作的

 * 
 */
void buildCmd(){
    cout<<"hello buildah-build!"<<endl;
}

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