#!/bin/bash
echo "Starting ociBuild test script..."

# 判断平台类型：Git Bash (MINGW/MSYS) 或 Linux
platform="linux"
unameOut="$(uname -s)"
case "${unameOut}" in
    MINGW*|MSYS*|CYGWIN*) platform="gitbash";;
esac
echo "Platform: $platform"
# 定义要测试的构建命令列表（用数组保存）
commands=(
  './output/ociBuild build --tag image:latest .'
  './output/ociBuild build --tag image:latest .'
  './output/ociBuild build --annotation "version=3.0" --annotation "author=NWPU" --tag image1:latest .'
  './output/ociBuild build --arch amd64 --tag image2:latest .'
  './output/ociBuild build --file Dockerfile.p1 --tag image3:latest .'
  './output/ociBuild build --os windows7 --tag image4:latest .'
  './output/ociBuild build --help'
  './output/ociBuild build --annotation "version=3.0" --annotation "author=NWPU" --arch amd64 --file Dockerfile.p1 --os windows7 --tag image5:latest .'
  './output/ociBuild build --check --tag image1:latest .'
)

# 输出文件夹
mkdir -p ./output/results
log_file="./output/results/build_log.txt"
# 循环执行每条命令
i=1
for cmd in "${commands[@]}"; do
    echo
    echo "[$i] Executing: $cmd"

    # 记录开始时间
    # start=$(date +%s)
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
    # end=$(date +%s)
    if [ "$platform" = "gitbash" ]; then
        end=$(date +%s)
    else
        end=$(date +%s.%N)
    fi

    # elapsed=$((end - start))
    # 计算耗时
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
echo "All build commands have been tested."
