Write-Host "Starting login command..."
Write-Host "Command: ./output/main.exe login --username admin --password 123456 192.168.1.102:5000"
# Execute the command
./output/main.exe login --username admin --password 123456 192.168.1.102:5000

Write-Host "Starting push command..."
Write-Host "Command: ./output/main.exe push 192.168.1.102:5000/library/image:latest"

$start = Get-Date

# Execute the command
./output/main.exe push 192.168.1.102:5000/library/image:latest

$end = Get-Date
$elapsed = $end - $start
$elapsedSeconds = $elapsed.TotalSeconds

$ImageSize = 428 + 562 + 49990647
$finalSizeMB = [math]::Round($ImageSize / 1MB, 3)

# Compute size speed
$speedMBps = if ($elapsedSeconds -gt 0) { [math]::Round($finalSizeMB / $elapsedSeconds, 3) } else { 0 }

Write-Host ("Push finished. Elapsed time: {0:N3} seconds" -f $elapsedSeconds)
Write-Host ("Final image size: {0} MB" -f $finalSizeMB)
Write-Host ("Average push throughput: {0} MB/s" -f $speedMBps)