#include <iostream>
#include "ParsingClass.h"
#include "MyImGui.h"
#include "Ti.h"



ParsingClass::ParsingClass()
{
	MyImGuis = std::make_shared<MyImGui>();
	Texas = std::make_shared<TI>();
}

ParsingClass::~ParsingClass()
{

}

void ParsingClass::ParsingStart()
{
	while(true)
	{
		switch (Datas)
		{
		case CompanyData::NONE:
		{
			std::cout << "���� ������ �߸� �ԷµǾ����ϴ�." << std::endl;
			std::cout << "���α׷��� ���� �� �ٽ� �Է����ּ���" << std::endl;
			break;
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
		}
	}
}
	