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
 * @brief 返回build命令的标志
 * <p>根据BuildOptions结构体中的标志名，创建并初始化每个flag
 * 
 * @param br br为BuildOptions的对象，存储build命令的flag
 * @return Flagset* 返回一个标志集指针，注意是动态分配，不就随着函数的结束而销毁
 */
Flagset* Getbuildflags(BuildOptions* br){
    //动态分配分配一个Flagset指针，用于存储初始化中的flag，并作为返回值
    Flagset* flags=new Flagset();
    //调用flag初始化函数
    flags->BoolVar((*br).allplatform,"all-platforms",false,"attempt to build for all base image platforms");
    flags->String("arch","amd64","set the ARCH of the image to the provided value instead of the architecture of the host");
    flags->StringArrayVar((*br).annotation,"annotation",vector<string>(),"set metadata for an image (default [])");
    flags->StringArrayVar((*br).tag,"tag",vector<string>(),"tagged `name` to apply to the built image");
    flags->StringVar((*br).osversion,"os-version","","set required OS `version` for the target image instead of the value from the base image");
    // flags.StringArrayVar(br.annotation,"",,"");
    // flags.String();
    return flags;
}

/**
 * @brief 返回图层的通用标志
 * 
 * @param lr 
 * @return Flagset 
 */
Flagset GetLayerFlags(LayerOptions& lr){
    Flagset flags;
    // flags.BoolVar();
    return flags;
}
/**
 * @brief 初始化build命令的内容
 * <p>新建一个Command对象用来初始化build命令，完成对build命令的flag的初始化，
 * 并且将初始化好的build对象加入到rootcmd中。
 * 
 */
void init_buildcmd(){
    BuildOptions* br=new BuildOptions();
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
    Command* build_Command=new Command(build_name,build_Short,build_Long,build_example);
    //定义使用模板
    string Template=UsageTemplate();
    build_Command->SetUsageTemplate(Template);
    build_Command->Args=MaximumNArgs(1);

    Flagset* flags=build_Command->Flags();
    flags->SetInterspersed(false);
    
    Flagset* buildflags=Getbuildflags(br);
    flags->AddFlagSet(buildflags);
    build_Command->Run=buildCmd;
    rootcmd.AddCommand({build_Command});
    // cout<<"hello buildah-build!"<<endl;
    // build_Command.Run();
    // return build_Command;
}

/**
 * @brief build 命令Run操作的
 * <p>定义build命令的行为
 * @param cmd 用来运行的命令
 * @param args 运行的参数
 */
void buildCmd(Command& cmd, vector<string> args){
    cout<<"hello buildah-build!"<<endl;
}
/**
 * @brief 模板定义
 * 
 * @return string 返回模板的字符串
 */
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