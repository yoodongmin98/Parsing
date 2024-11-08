#include "SerialTimeStamp.h"
#include "serial/serial.h"
#include "Core.h"
#include <string>
#include <iostream>
#include "Time.h"


SerialTimeStamp::SerialTimeStamp()
{

}

SerialTimeStamp::~SerialTimeStamp()
{

}


//나중에 시간을 담당하는 class하나만들면될듯
void SerialTimeStamp::Instance()
{
	//시리얼 통신을 설정합니다
	serial::Serial my_serial(Core::Cores->GetPortNumber(), Core::Cores->GetBaudrate(), serial::Timeout::simpleTimeout(1000));
	


	Time::Times->TimeCheckStart();
	int no_data_count = 0; 
	while (true)
	{
		Time::Times->TimeCheckEnd();
		auto elapsed = Time::Times->GetProgressTime();
		std::string Dataline;
		try {
			Dataline = my_serial.readline();
		}
		catch (const std::exception& e) {
			std::cout << std::endl << std::endl;
			std::cout << "시리얼 통신이 끊겼습니다. " << e.what() << std::endl;
			break; 
		}
		
		if (!Dataline.empty())
		{
			std::cout << "\n\n세계 표준 날짜: " << Time::Times->GetLocalDay() << std::endl;
			std::cout << "한국 표준 시간: " << Time::Times->GetLocalTime() << std::endl;
			std::cout << "데이터 출력: " << Dataline;
			std::cout << "데이터가 정상수신되지않은 연속 횟수 : " << Max_NoData_Count << std::endl;
			Nodata_Time_Count = elapsed;
			no_data_count = 0; 
		}

		if (Max_NoData_Count == 5)
		{
			std::cout << std::endl << "데이터가 5번이상 정상 수신되지 않아 프로그램을 종료합니다." << std::endl;
			return;
		}
		if (elapsed - Nodata_Time_Count > 10)
		{
			no_data_count++;
			Max_NoData_Count=std::max(Max_NoData_Count, no_data_count);
			Nodata_Time_Count = elapsed;
			std::cout << std::endl << "10초간 데이터가 수신되지 않았습니다. 다시 시도합니다 현재 연속 수신되지 않은횟수 :" << no_data_count << std::endl;
		}
		std::cout << "\r진행된 시간: " << elapsed << " 초" << std::flush;
	}
}
