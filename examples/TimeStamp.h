#pragma once
#include <iostream>



class TimeStamp
{
public:
	TimeStamp();
	~TimeStamp();

	void Instance();
protected:
private:
	int Max_NoData_Count = 0;
	int Nodata_Time_Count = 0;
};