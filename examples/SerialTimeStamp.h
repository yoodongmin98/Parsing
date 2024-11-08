#pragma once
#include <iostream>



class SerialTimeStamp
{
public:
	SerialTimeStamp();
	~SerialTimeStamp();

	void Instance();
protected:
private:
	int Max_NoData_Count = 0;
	int Nodata_Time_Count = 0;
};