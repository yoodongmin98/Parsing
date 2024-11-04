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
			std::cout << "관련 정보가 잘못 입력되었습니다." << std::endl;
			std::cout << "프로그램을 종료 후 다시 입력해주세요" << std::endl;
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
	