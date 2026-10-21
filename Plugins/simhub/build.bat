@echo off
setlocal
cd /d "%~dp0"

where cmake >nul 2>nul
if errorlevel 1 (
  echo [FAIL] cmake was not found in PATH.
  goto :fail
)

echo [INFO] Configuring Release build...
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
  echo [FAIL] CMake configuration failed.
  goto :fail
)

echo [INFO] Compiling Release build...
cmake --build build --config Release --target ALL_BUILD
if errorlevel 1 (
  echo [FAIL] Release compilation failed.
  goto :fail
)

echo [INFO] Deploying Release plugin...
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\deploy.ps1"
if errorlevel 1 (
  echo [FAIL] Release deployment failed.
  goto :fail
)

echo.
title SUCCESS - Stream Dock Plugin Deployed
color 2F
echo ========================================================
echo [SUCCESS] Release plugin built and deployed successfully.
echo ========================================================
echo This window will close in 2 seconds...
powershell -NoProfile -Command "Start-Sleep -Seconds 2"
exit /b 0

:fail
set "RESULT=%errorlevel%"
if "%RESULT%"=="0" set "RESULT=1"
echo [FAIL] Exit code: %RESULT%
echo.
pause
exit /b %RESULT%
