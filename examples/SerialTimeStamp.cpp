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


//���߿� �ð��� ����ϴ� class�ϳ������ɵ�
void SerialTimeStamp::Instance()
{
	//�ø��� ����� �����մϴ�
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
			std::cout << "�ø��� ����� ������ϴ�. " << e.what() << std::endl;
			break; 
		}
		
		if (!Dataline.empty())
		{
			std::cout << "\n\n���� ǥ�� ��¥: " << Time::Times->GetLocalDay() << std::endl;
			std::cout << "�ѱ� ǥ�� �ð�: " << Time::Times->GetLocalTime() << std::endl;
			std::cout << "������ ���: " << Dataline;
			std::cout << "�����Ͱ� ������ŵ������� ���� Ƚ�� : " << Max_NoData_Count << std::endl;
			Nodata_Time_Count = elapsed;
			no_data_count = 0; 
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
