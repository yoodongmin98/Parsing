#include <iostream>
#include "ParsingClass.h"
#include "MyImGui.h"
#include "Ti.h"
#include "TimeStamp.h"


ParsingClass::ParsingClass()
{
	MyImGuis = std::make_shared<MyImGui>();
	Texas = std::make_shared<TI>();
	UART_TimeStamp = std::make_shared<TimeStamp>();
}

ParsingClass::~ParsingClass()
{

}

void ParsingClass::ParsingStart()
{
		switch (Datas)
		{
		default:
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
		}
}
	