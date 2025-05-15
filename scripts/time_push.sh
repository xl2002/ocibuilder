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
  # 天脉仓库推送
  './output/ociBuild push 192.168.1.102:5000/library/image:1.0'
  'buildah push --format docker 192.168.1.102:5000/library/image:latest'
  'docker push 192.168.1.102:5000/library/image:latest'

  # harbor仓库推送
  ./output/ociBuild push 192.168.1.102:80/library/image:1.0
  'buildah push 192.168.1.102:80/library/image:latest'
  'docker push 192.168.1.102:80/library/image:latest'
)
# 准备阶段
./output/ociBuild build --tag image1:latest .
# harbor 仓库登录
./output/ociBuild login --username admin --password Harbor12345 192.168.1.102:80
docker login --username admin --password Harbor12345 192.168.1.102:80
buildah login --username admin --password Harbor12345 --tls-verify=false 192.168.1.102:80

# 天脉仓库登录
./output/ociBuild login --username admin --password 123456 192.168.1.102:5000
docker login --username admin --password 123456 192.168.1.102:5000
buildah login --username admin --password 123456 --tls-verify=false 192.168.1.102:5000

# 添加tag
buildah tag image:latest 192.168.1.102:5000/library/image:latest
buildah tag image:latest 192.168.1.102:80/library/image:latest
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
    # 镜像大小要测试人员自己计算
    ImageSize=$((428 + 562 + 29354605))
    # 转换为 MB，scale=3 表示保留3位小数
    finalSizeMB=$(echo "scale=3; $ImageSize / 1048576" | bc)
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
    echo "Push finished. Elapsed time: $elapsed seconds"
    echo "Final image size: $finalSizeMB MB"
    echo "Average push throughput: $speed MB/s"
    if [ $status -eq 0 ]; then
        echo "✅ Command [$i] succeeded in ${elapsed}s"
    else
        echo "❌ Command [$i] failed with exit code $status after ${elapsed}s"
    fi
    ((i++))
done

echo
echo "All push commands have been tested."
