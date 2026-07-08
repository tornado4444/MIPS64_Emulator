@echo off
setlocal

set "BUILD_EXE=build.exe"
set "BUILD_SRC=build.c"
set "NEED_REBUILD=0"

if not exist "%BUILD_EXE%" set "NEED_REBUILD=1"

if "%NEED_REBUILD%"=="0" (
    powershell -NoProfile -ExecutionPolicy Bypass -Command "if ((Get-Item -LiteralPath '%BUILD_SRC%').LastWriteTimeUtc -gt (Get-Item -LiteralPath '%BUILD_EXE%').LastWriteTimeUtc) { exit 0 } else { exit 1 }"
    if not errorlevel 1 set "NEED_REBUILD=1"
)

if "%NEED_REBUILD%"=="1" goto rebuild
goto run

:rebuild
where clang >nul 2>nul
if %errorlevel%==0 (
    clang -std=c11 -Wall -Wextra -o "%BUILD_EXE%" "%BUILD_SRC%"
    if errorlevel 1 exit /b 1
    goto run
)

where gcc >nul 2>nul
if %errorlevel%==0 (
    gcc -std=c11 -Wall -Wextra -o "%BUILD_EXE%" "%BUILD_SRC%"
    if errorlevel 1 exit /b 1
    goto run
)

where cl >nul 2>nul
if %errorlevel%==0 (
    cl /nologo /TC /std:c11 "%BUILD_SRC%" /Fe:"%BUILD_EXE%"
    if errorlevel 1 exit /b 1
    goto run
)

echo [ERROR] No suitable compiler found (clang, gcc, or cl).
exit /b 1

:run
"%BUILD_EXE%" %*
exit /b %errorlevel%
