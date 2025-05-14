#!/bin/bash
# 判断平台类型：Git Bash (MINGW/MSYS) 或 Linux
platform="linux"
unameOut="$(uname -s)"
case "${unameOut}" in
    MINGW*|MSYS*|CYGWIN*) platform="gitbash";;
esac
echo "Platform: $platform"

# 定义要测试的构建命令列表（用数组保存）
commands=(
    './output/ociBuild push 192.168.1.102:80/library/image1:latest'
# 指定格式
    './output/ociBuild push --format v1 192.168.1.102:80/library/image0401:latest'
# 推送镜像到本地目录
    './output/ociBuild push image1:latest oci:./test/image1:imagetest:latest'
)
# 准备阶段
./output/ociBuild build --tag image1:latest .
./output/ociBuild login --username admin --password Harbor12345 192.168.1.102:80
# ./output/ociBuild push image1:latest oci:./test/image:imagetest:latest
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

    # 执行命令
    eval "$cmd"
    # eval "$cmd" >> "build_log_$i.txt" 2>&1
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
echo "All push commands have been tested."
