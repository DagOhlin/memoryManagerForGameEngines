To build the project, run either ./build.bat (for windows) or ./build.sh (for linux). You need CMake 3.11 or later, and C++ 20 or later. Using GCC as compiler is preferred.
To run it, use ./run.bat (for windows) or ./run.sh (for linux).

To run it multithreaded, you need to uncomment #define MULTITHREAD_SUPPORT in headers/allocator.hpp