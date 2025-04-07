#!/bin/bash

echo "Starting login command..."
echo "Command: ./output/main.exe login --username admin --password 123456 192.168.1.102:5000"

# Execute the login command
./output/main login --username admin --password 123456 192.168.1.102:5000

echo "Starting push command..."
echo "Command: ./output/main.exe push 192.168.1.102:5000/library/image:latest"

# Record start time
start=$(date +%s)

# Execute the push command
./output/main push 192.168.1.102:5000/library/image:latest

# Record end time
end=$(date +%s)

# Calculate elapsed time in seconds
elapsed=$((end - start))

# Image size in bytes (428 + 562 + 29354770)
ImageSize=$((428 + 562 + 29354770))

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
