#include "Pch.h"
#include "Timer.h"

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::Start()
{
    _startRecord = std::chrono::steady_clock::now();
}

double Timer::Read() const
{
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(now - _startRecord);
    return duration.count();
}