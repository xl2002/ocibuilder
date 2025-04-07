@echo off
setlocal enabledelayedexpansion

echo Starting login command...
echo Command: .\output\main.exe login --username admin --password 123456 192.168.1.102:5000

REM 执行登录命令
.\output\main.exe login --username admin --password 123456 192.168.1.102:5000

echo Starting push command...
echo Command: .\output\main.exe push 192.168.1.102:5000/library/image:latest

REM 记录开始时间
for /f "tokens=1-4 delims=:.," %%a in ("%time%") do (
    set /a start=%%a*3600 + %%b*60 + %%c
)

REM 执行推送命令
.\output\main.exe push 192.168.1.102:5000/library/image:latest

REM 记录结束时间
for /f "tokens=1-4 delims=:.," %%a in ("%time%") do (
    set /a end=%%a*3600 + %%b*60 + %%c
)

set /a duration=%end% - %start%
if %duration% LSS 0 (
    set /a duration=%duration% + 86400
)

REM 获取最终镜像大小（单位：MB，浮点数）
for /f %%a in ('powershell -nologo -noprofile -command ^ 
    "(428 + 562 + 49990647) / 1MB"') do (
    set finalSizeMB=%%a
)

REM 计算推送效率（浮点数除法）
for /f %%a in ('powershell -nologo -noprofile -command ^ 
    "[math]::Round(%finalSizeMB% / %duration%, 3)"') do (
    set speed=%%a
)

echo Push finished. Elapsed time: %duration% seconds
echo Final image size: %finalSizeMB% MB
echo Average push throughput: !speed! MB/s
endlocal
pause
