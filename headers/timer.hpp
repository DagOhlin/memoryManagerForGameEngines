#pragma once

#include <chrono>
#include <iostream>
#include <string>

//times a function from point of creation to when it dies aka end of func

class ScopeTimer
{
public:
    using Clock = std::chrono::steady_clock;

    explicit ScopeTimer(std::string label)
        : label(std::move(label)), output(nullptr), start(Clock::now()) {}

    explicit ScopeTimer(double &outputMs)
        : output(&outputMs), start(Clock::now()) {}

    ~ScopeTimer()
    {
        const double ms = std::chrono::duration<double, std::milli>(Clock::now() - start).count();
        if (output)
            *output = ms;
        else
            std::cout << label << ": " << ms << " ms\n";
    }

    ScopeTimer(const ScopeTimer &) = delete;
    ScopeTimer &operator=(const ScopeTimer &) = delete;

private:
    std::string label;
    double *output;
    Clock::time_point start;
};