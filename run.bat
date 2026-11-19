@echo off

set "EXE="

:: Different path for different compiler
if exist "%~dp0build\memoryManager.exe" (
    set "EXE=%~dp0build\memoryManager.exe"
) else if exist "%~dp0build\Debug\MemoryManager.exe" (
    set "EXE=%~dp0build\Debug\MemoryManager.exe"
)

if not defined EXE (
    echo executable file didn't exist, build first
    exit /b 1
)

call "%EXE%"
