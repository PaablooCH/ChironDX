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
	_startRecord = std::chrono::high_resolution_clock::now();
}

float Timer::Read() const
{
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - _startRecord);
    return static_cast<float>(duration.count());
}
