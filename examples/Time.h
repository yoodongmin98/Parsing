#pragma once
#include <string>
#include <ctime>
#include <chrono>
#include <iostream>
class Time
{
public:
	Time();
	~Time();
	static Time* Times;

	void TimeCheckStart();
	void TimeCheckEnd();
	int GetProgressTime();
	std::string GetLocalTime();
	std::string GetLocalDay();
protected:
private:
	std::chrono::system_clock::time_point StartTime;
	std::chrono::system_clock::time_point EndTime;
	int ProgressTime = 0;

	__time64_t now2;
	tm tm_2;
};