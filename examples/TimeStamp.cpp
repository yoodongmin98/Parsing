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

			last_data_time = current_time; 
			no_data_count = 0; 
		}

		static auto no_data_elapsed = duration_cast<seconds>(current_time - last_data_time).count();

		if (no_data_elapsed >= 7)
		{
			no_data_count++; 
			last_data_time = current_time; 
			std::cout << "\n 7초 동안 데이터가 수신되지 않았습니다." << no_data_count << std::endl;
			break;
		}

		std::cout << "\r진행된 시간: " << elapsed << " 초" << std::flush;
	}
}
