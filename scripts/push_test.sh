#!/bin/bash
echo "Starting login command..."
echo "Command: login --username admin --password 123456 192.168.1.102:5000"

# Execute the login command
# harbor 仓库登录
# ./output/main login --username admin --password Harbor12345 192.168.1.102:80
# docker login --username admin --password Harbor12345 192.168.1.102:80
# buildah login --username admin --password Harbor12345 --tls-verify=false 192.168.1.102:80

# 天脉仓库登录
# ./output/main login --username admin --password 123456 192.168.1.102:5000
# docker login --username admin --password 123456 192.168.1.102:5000
# buildah login --username admin --password 123456 --tls-verify=false 192.168.1.102:5000

echo "Starting push command..."
# echo "Command: buildah push 192.168.1.102:5000/library/image:latest"
echo "Command: push 192.168.1.102:80/library/image:latest"
# Record start time
start=$(date +%s)

# Execute the push command
# 天脉仓库推送
# ./output/main push 192.168.1.102:5000/library/image:1.0
# buildah push --format docker 192.168.1.102:5000/library/image:latest
# docker push 192.168.1.102:5000/library/image:latest

# harbor仓库推送
# ./output/main push 192.168.1.102:80/library/image:1.0
# buildah push 192.168.1.102:80/library/image:latest
docker push 192.168.1.102:80/library/image:latest


# Record end time
end=$(date +%s)

# Calculate elapsed time in seconds
elapsed=$((end - start))

# Image size in bytes (428 + 562 + 29354770)
ImageSize=$((428 + 562 + 29354605))

# Convert image size to MB
finalSizeMB=$(echo "scale=3; $ImageSize / 1048576" | bc)

# Compute speed (MB/s)
if [ $elapsed -gt 0 ]; then
  speedMBps=$(echo "scale=3; $finalSizeMB / $elapsed" | bc)
else
  speedMBps=0
fi

echo "Push finished. Elapsed time: $elapsed seconds"
echo "Final image size: $finalSizeMB MB"
echo "Average push throughput: $speedMBps MB/s"
