#pragma once

class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	float Read() const;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _startRecord;
};
