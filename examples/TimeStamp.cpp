#include "TimeStamp.h"
#include "serial/serial.h"
#include "Core.h"
#include "string.h"
#include <ctime>
#include <chrono>
#include <iostream>


using namespace std::chrono;
TimeStamp::TimeStamp()
{

}

TimeStamp::~TimeStamp()
{

}


//나중에 시간을 담당하는 class하나만들면될듯
void TimeStamp::Instance()
{
	//시리얼 통신을 설정합니다
	serial::Serial my_serial;
	my_serial.setPort(Core::Cores->GetPortNumber());
	my_serial.setBaudrate(Core::Cores->GetBaudrate());
	my_serial.setTimeout(serial::Timeout::simpleTimeout(1000));
	try {
		my_serial.open();
	}
	catch (const std::exception& e) {
		std::cerr << "포트를 열 수 없습니다: " << e.what() << std::endl;
		return;
	}

	system_clock::time_point start_time = system_clock::now();
	int no_data_count = 0; 
	system_clock::time_point last_data_time = start_time; 
	while (true)
	{
		auto current_time = system_clock::now();
		auto elapsed = duration_cast<seconds>(current_time - start_time).count();
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
			__time64_t now2 = _time64(nullptr);
			tm tm_2;
			localtime_s(&tm_2, &now2);
			std::cout << "\n\n세계 표준 날짜: " << (tm_2.tm_year + 1900) << "/" << (tm_2.tm_mon + 1) << "/" << tm_2.tm_mday << std::endl;
			std::cout << "한국 표준 시간: " << tm_2.tm_hour << ":" << tm_2.tm_min << ":" << tm_2.tm_sec << std::endl;
			std::cout << "데이터 출력: " << Dataline;
			std::cout << "데이터가 정상수신되지않은 연속 횟수 : " << Max_NoData_Count << std::endl;
			Nodata_Time_Count = elapsed;
			no_data_count = 0; 
			
			last_data_time = current_time; 
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
