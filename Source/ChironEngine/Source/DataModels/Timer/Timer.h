#pragma once

class Timer
{
public:
    Timer();
    ~Timer();

    void Start();
    double Read() const;

private:
    std::chrono::time_point<std::chrono::steady_clock> _startRecord;
};
