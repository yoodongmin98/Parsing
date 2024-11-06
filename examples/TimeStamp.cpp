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
	//�ø��� ����� �����մϴ�
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
			std::cout << "�����Ͱ� ������ŵ������� ���� Ƚ�� : " << Max_NoData_Count << std::endl;
			Nodata_Time_Count = elapsed;
			no_data_count = 0; 
			
			last_data_time = current_time; 
		}

		if (Max_NoData_Count == 5)
		{
			std::cout << std::endl << "�����Ͱ� 5���̻� ���� ���ŵ��� �ʾ� ���α׷��� �����մϴ�." << std::endl;
			return;
		}
		if (elapsed - Nodata_Time_Count > 10)
		{
			no_data_count++;
			Max_NoData_Count=std::max(Max_NoData_Count, no_data_count);
			Nodata_Time_Count = elapsed;
			std::cout << std::endl << "10�ʰ� �����Ͱ� ���ŵ��� �ʾҽ��ϴ�. �ٽ� �õ��մϴ� ���� ���� ���ŵ��� ����Ƚ�� :" << no_data_count << std::endl;
		}
		std::cout << "\r����� �ð�: " << elapsed << " ��" << std::flush;
	}
}
