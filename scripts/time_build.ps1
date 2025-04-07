Write-Host "Starting build command..."
Write-Host "Command: ./output/main.exe build --tag image:latest ."

$start = Get-Date

# Execute the command
./output/main.exe build --tag image:latest .

$end = Get-Date
$elapsed = $end - $start
$elapsedSeconds = $elapsed.TotalSeconds

# Calculate final size of ./app (MB)
$SizeBytes = (Get-ChildItem -Recurse -File "./app" | Measure-Object -Property Length -Sum).Sum
$finalSizeMB = [math]::Round($SizeBytes / 1MB, 3)

# Compute size speed
$speedMBps = if ($elapsedSeconds -gt 0) { [math]::Round($finalSizeMB / $elapsedSeconds, 3) } else { 0 }

Write-Host ("Build finished. Elapsed time: {0:N3} seconds" -f $elapsedSeconds)
Write-Host ("Final ./app size: {0} MB" -f $finalSizeMB)
Write-Host ("Average build throughput: {0} MB/s" -f $speedMBps)
