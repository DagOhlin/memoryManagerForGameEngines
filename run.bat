@echo off

if exist "build/memoryManager.exe" (
        call "build/memoryManager.exe"
    ) else (
        echo executable file didn't exist, build first
    )
