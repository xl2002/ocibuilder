@echo off
setlocal enabledelayedexpansion

echo Starting build command...
echo Command: .\output\main.exe build --tag image:latest .

REM 记录开始时间
for /f "tokens=1-4 delims=:.," %%a in ("%time%") do (
    set /a start=%%a*3600 + %%b*60 + %%c
)

REM 执行构建命令
.\output\main.exe build --tag image:latest .

REM 记录结束时间
for /f "tokens=1-4 delims=:.," %%a in ("%time%") do (
    set /a end=%%a*3600 + %%b*60 + %%c
)

set /a duration=%end% - %start%
if %duration% LSS 0 (
    set /a duration=%duration% + 86400
)

REM 获取 ./app 文件夹大小（单位：MB，浮点数）
for /f %%a in ('powershell -nologo -noprofile -command ^
    "(Get-ChildItem -Recurse -File './app' | Measure-Object -Property Length -Sum).Sum / 1MB"') do (
    set finalSizeMB=%%a
)

REM 计算构建效率（浮点数除法）
for /f %%a in ('powershell -nologo -noprofile -command ^
    "[math]::Round(%finalSizeMB% / %duration%, 3)"') do (
    set speed=%%a
)

echo Build finished. Elapsed time: %duration% seconds
echo Final ./app size: %finalSizeMB% MB
echo Average build throughput: !speed! MB/s
endlocal
pause
