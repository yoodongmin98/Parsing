#include <iostream>
#include "ParsingClass.h"
#include "MyImGui.h"



ParsingClass::ParsingClass()
{
	MyImGuis = std::make_shared<MyImGui>();
}

ParsingClass::~ParsingClass()
{

}

void ParsingClass::ParsingStart()
{
	switch (Datas)
	{
	case CompanyData::NONE :
	{
		std::cout << "���� ȸ�� ������ �߸� �ԷµǾ����ϴ�." << std::endl;
		std::cout << "���α׷��� ���� �� �ٽ� �Է����ּ���" << std::endl;
		break;
	}
	case CompanyData::IQDATA :
	{
		MyImGuis->Instance();
		break;
	}
	case CompanyData::TI :
	{

		break;
	}
	}
}