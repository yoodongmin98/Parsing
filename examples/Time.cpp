#include "Time.h"
#include <ctime>
#include <chrono>
#include <iostream>
#include <string>



using namespace std::chrono;




Time* Time::Times = nullptr;

Time::Time()
{
	Times = this;
}

Time::~Time()
{
	
}


void Time::TimeCheckStart()
{
	StartTime = system_clock::now();
}


void Time::TimeCheckEnd()
{
	EndTime = system_clock::now();
}


int Time::GetProgressTime()
{
	ProgressTime = std::chrono::duration_cast<seconds>(EndTime - StartTime).count();
	return ProgressTime;
}


std::string Time::GetLocalTime()
{
	now2 = _time64(nullptr);
	localtime_s(&tm_2, &now2);
	std::string LocalTime = std::to_string(tm_2.tm_hour) + ":" + std::to_string(tm_2.tm_min) + ":" + std::to_string(tm_2.tm_sec);
	return LocalTime;
}


std::string Time::GetLocalDay()
{
	now2 = _time64(nullptr);
	localtime_s(&tm_2, &now2);
	std::string LocalDay = std::to_string((tm_2.tm_year + 1900)) + "³â" + std::to_string((tm_2.tm_mon + 1)) + "¿ù" + std::to_string((tm_2.tm_mday)) + "ÀÏ";
	return LocalDay;
}