#include "Core.h"
#include "ParsingClass.h"
#include <iostream>
#include <string>
#include "CSV.h"



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

	std::cout << "��Ʈ ��ȣ�� �Է��ϼ���(COM���� ���ڸ�) : ";
	std::cin >> PortNum;
	std::cout << "Baudrate�� �Է��ϼ��� : ";
	std::cin >> Baudrate;
	std::cout << std::endl << "Ȯ�� �� �����͸� �Է��ϼ���" << std::endl;
	std::cout << "[ IQ������ �ð�ȭ : 1 ]" << std::endl << "[ TI UART DATA Ȯ�� : 2 ]" << std::endl;
	std::cout << "[ Data Timestamp : 3 ]" << std::endl;
	std::cout << "[ FV Data CSV file���� : 4 ]" << std::endl;
	std::cout << "�Է� : ";
	std::cin >> Company;
	Classes->SetCompanyData(static_cast<CompanyData>(Company));


	//��Ʈ�� baudrate��������
	port = "COM"+ PortNum;
	baud = Baudrate;
	Classes->ParsingStart();
}