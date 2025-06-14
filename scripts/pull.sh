#!/bin/bash
# 判断平台类型：Git Bash (MINGW/MSYS) 或 Linux
platform="linux"
unameOut="$(uname -s)"
case "${unameOut}" in
    MINGW*|MSYS*|CYGWIN*) platform="gitbash";;
esac
echo "Platform: $platform"

ip_addr="192.168.253.128:5000"
# 定义要测试的构建命令列表（用数组保存）
commands=(
    "./output/ociBuild pull ${ip_addr}/library/image:latest"
    "./output/ociBuild rmi image:latest"
    # "./output/ociBuild pull --format v1 ${ip_addr}/library/image:latest "
    # "./output/ociBuild rmi image:latest"
    "./output/ociBuild pull --os linux --arch amd64 ${ip_addr}/library/image:latest"
    "./output/ociBuild rmi image:latest"
    "./output/ociBuild pull --os linux --arch amd64 --all-tags ${ip_addr}/library/image"
    "./output/ociBuild rmi image:latest"
    './output/ociBuild pull oci:./test/image:image:latest'
    "./output/ociBuild rmi image:latest"
    './output/ociBuild pull --help'
)
# 准备阶段
# 输出文件夹
mkdir -p ./output/results
log_file="./output/results/pull_log.txt"
# 循环执行每条命令
i=1
for cmd in "${commands[@]}"; do
    echo
    echo "[$i] Executing: $cmd"

    # 记录开始时间
    if [ "$platform" = "gitbash" ]; then
        start=$(date +%s)
    else
        start=$(date +%s.%N)
    fi

    # 执行命令, 第一次写入为覆盖
    if [ "$i" -eq 1 ]; then
        : > "${log_file}"
    else
        echo "" >> "${log_file}"
    fi
    # 执行命令
    echo "[$i] Executing: $cmd" >> "${log_file}"
    eval "$cmd" >> "${log_file}" 2>&1
    status=$?

    # 记录结束时间
    if [ "$platform" = "gitbash" ]; then
        end=$(date +%s)
    else
        end=$(date +%s.%N)
    fi

    if command -v bc >/dev/null 2>&1; then
        elapsed=$(echo "$end - $start" | bc)
    else
        # fallback for Git Bash without bc
        elapsed=$((end - start))
    fi

    if [ $status -eq 0 ]; then
        echo "✅ Command [$i] succeeded in ${elapsed}s"
    else
        echo "❌ Command [$i] failed with exit code $status after ${elapsed}s"
    fi
    ((i++))
done

echo
echo "All pull commands have been tested."
