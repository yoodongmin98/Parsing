#include "Core.h"
#include "ParsingClass.h"
#include <iostream>
#include <string>




Core* Core::Cores = nullptr;


Core::Core()
{
	Cores = this;
	Classes = std::make_shared<ParsingClass>();
}
Core::~Core()
{
	
}


void Core::Start()
{
	std::string PortNum;
	unsigned int Baudrate;
	unsigned int Company;

	std::cout << "포트 번호를 입력하세요(COM제외 숫자만) : ";
	std::cin >> PortNum;
	std::cout << "Baudrate를 입력하세요 : ";
	std::cin >> Baudrate;
	std::cout << "칩 회사명을 선택해주세요" << std::endl;
	std::cout << "[ IQ데이터 확인 : 1 ] [ TI : 2 ]" << std::endl;
	std::cin >> Company;
	Classes->SetCompanyData(static_cast<CompanyData>(Company));


	//포트와 baudrate설정저장
	port = "COM"+ PortNum;
	baud = Baudrate;
	Classes->ParsingStart();
}