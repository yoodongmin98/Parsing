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


//���߿� �ð��� ����ϴ� class�ϳ������ɵ�
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
		std::cerr << "��Ʈ�� �� �� �����ϴ�: " << e.what() << std::endl;
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
			std::cout << "�ø��� ����� ������ϴ�. " << e.what() << std::endl;
			break; 
		}

		if (!Dataline.empty())
		{
			__time64_t now2 = _time64(nullptr);
			tm tm_2;
			localtime_s(&tm_2, &now2);
			std::cout << "\n\n���� ǥ�� ��¥: " << (tm_2.tm_year + 1900) << "/" << (tm_2.tm_mon + 1) << "/" << tm_2.tm_mday << std::endl;
			std::cout << "�ѱ� ǥ�� �ð�: " << tm_2.tm_hour << ":" << tm_2.tm_min << ":" << tm_2.tm_sec << std::endl;
			std::cout << "������ ���: " << Dataline;

			last_data_time = current_time; 
			no_data_count = 0; 
		}

		static auto no_data_elapsed = duration_cast<seconds>(current_time - last_data_time).count();

		if (no_data_elapsed >= 7)
		{
			no_data_count++; 
			last_data_time = current_time; 
			std::cout << "\n 7�� ���� �����Ͱ� ���ŵ��� �ʾҽ��ϴ�." << no_data_count << std::endl;
			break;
		}

		std::cout << "\r����� �ð�: " << elapsed << " ��" << std::flush;
	}
}
