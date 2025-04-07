#!/bin/bash

echo "Starting build command..."
echo "Command: ./output/main.exe build --tag image:latest ."

# 记录开始时间（秒.纳秒）
start=$(date +%s.%N)

# 执行构建命令
./output/main build --tag image:latest .

# 记录结束时间
end=$(date +%s.%N)

# 计算耗时（浮点数，秒）
elapsed=$(echo "$end - $start" | bc)

# 获取 ./app 文件夹大小（MB）
finalSizeMB=$(du -sm ./app 2>/dev/null | cut -f1)

# 计算构建速度（MB/s）
if (( $(echo "$elapsed > 0" | bc -l) )); then
    speed=$(echo "scale=3; $finalSizeMB / $elapsed" | bc)
else
    speed=0
fi

echo "Build finished. Elapsed time: ${elapsed} seconds"
echo "Final ./app size: ${finalSizeMB} MB"
echo "Average build throughput: ${speed} MB/s"
