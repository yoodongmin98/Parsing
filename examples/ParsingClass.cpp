#include <iostream>
#include "ParsingClass.h"
#include "MyImGui.h"
#include "Ti.h"
#include "SerialTimeStamp.h"
#include "CSV.h"

ParsingClass::ParsingClass()
{
	MyImGuis = std::make_shared<MyImGui>();
	Texas = std::make_shared<TI>();
	UART_TimeStamp = std::make_shared<SerialTimeStamp>();
	CSVFile = std::make_shared<CSV>();
}

ParsingClass::~ParsingClass()
{

}

void ParsingClass::ParsingStart()
{
		switch (Datas)
		{
		case CompanyData::NONE:
		{
			std::cout << "���� ������ �߸� �ԷµǾ����ϴ�." << std::endl;
			std::cout << "���α׷��� ���� �� �ٽ� �Է����ּ���" << std::endl;
			return;
		}
		case CompanyData::IQDATA:
		{
			MyImGuis->Instance();
			break;
		}
		case CompanyData::TI:
		{
			Texas->Instance();
			break;
		}
		case CompanyData::TIMESTAMP :
		{
			UART_TimeStamp->Instance();
			break;
		}
		case CompanyData::CSV:
		{
			int InputSize = 0;
			std::cout << "������ �������� ����� �Է����ּ���" << std::endl;
			std::cin >> InputSize;
			CSVFile->Instance(InputSize);
			break;
		}
		default:
		{
			std::cout << "������ �߸� �ԷµǾ����ϴ�" << std::endl;
			return;
		}
		}
}
	