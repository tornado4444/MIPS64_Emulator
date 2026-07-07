@echo off

if not exist build.exe (
    cl /nologo /TC build.c /Fe:build.exe
)
if not exist build.exe (
    gcc build.c -o build.exe 2>nul
) else(
    clang build.c -o build.exe 2>nul  
)

if exist build.exe ( 
    build.exe %*
) else (
    echo [ERROR] No suitable compiler found (cl, gcc, or clang).
    exit \b 1
)
