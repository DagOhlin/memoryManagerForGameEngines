@echo off

if not exist "build\CMakeCache.txt" (
    if /i "%1"=="Ninja" (
        call cmake -B build -G Ninja || exit /b
    ) else (
        call cmake -B build || exit /b
    )
)

call cmake --build build || exit /b