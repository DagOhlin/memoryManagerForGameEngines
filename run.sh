#!/usr/bin/env bash
#kom ihåg chmod +x run.sh
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EXE=""

# Different paths/casing depending on generator or CMake target name
if [ -f "$SCRIPT_DIR/build/memoryManager" ]; then
    EXE="$SCRIPT_DIR/build/memoryManager"
elif [ -f "$SCRIPT_DIR/build/MemoryManager" ]; then
    EXE="$SCRIPT_DIR/build/MemoryManager"
elif [ -f "$SCRIPT_DIR/build/Debug/memoryManager" ]; then
    EXE="$SCRIPT_DIR/build/Debug/memoryManager"
elif [ -f "$SCRIPT_DIR/build/Debug/MemoryManager" ]; then
    EXE="$SCRIPT_DIR/build/Debug/MemoryManager"
fi

if [ -z "$EXE" ]; then
    echo "executable file didn't exist, build first" >&2
    exit 1
fi

"$EXE"