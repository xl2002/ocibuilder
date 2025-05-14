#!/bin/bash

echo "Starting build command..."
# echo "Command: ./output/ociBuild build --tag image:latest ."

# 获取当前平台（判断是否为 Git Bash）
platform="linux"
unameOut="$(uname -s)"
case "${unameOut}" in
    MINGW*|MSYS*|CYGWIN*) platform="gitbash";;
esac

# 获取开始时间（以秒为单位的小数）
if [ "$platform" = "gitbash" ]; then
    start=$(date +%s)
else
    start=$(date +%s.%N)
fi

# 执行构建命令
./output/ociBuild build --tag image:latest .

# 获取结束时间
if [ "$platform" = "gitbash" ]; then
    end=$(date +%s)
else
    end=$(date +%s.%N)
fi

# 计算耗时
if command -v bc >/dev/null 2>&1; then
    elapsed=$(echo "$end - $start" | bc)
else
    # fallback for Git Bash without bc
    elapsed=$((end - start))
fi

# 获取 ./app 文件夹大小（MB）
if [ "$platform" = "gitbash" ]; then
    finalSizeKB=$(du -sk ./app 2>/dev/null | cut -f1)
    finalSizeMB=$((finalSizeKB / 1024))
else
    finalSizeMB=$(du -sm ./app 2>/dev/null | cut -f1)
fi

# 计算构建速度（MB/s）
if command -v bc >/dev/null 2>&1; then
    if (( $(echo "$elapsed > 0" | bc -l) )); then
        speed=$(echo "scale=3; $finalSizeMB / $elapsed" | bc)
    else
        speed=0
    fi
else
    if [ "$elapsed" -gt 0 ]; then
        speed=$((finalSizeMB / elapsed))
    else
        speed=0
    fi
fi

echo "Build finished. Elapsed time: ${elapsed} seconds"
echo "Final ./app size: ${finalSizeMB} MB"
echo "Average build throughput: ${speed} MB/s"
