#include <iostream>
#include <string>
#include "Core.h"
#include "Calculator.h"
#include "serial/serial.h"






Core::Core()
{
	Calculators = std::make_shared<Calculator>();
}
Core::~Core()
{
	
}


void Core::Start(int argc, char** argv)
{
	port = "COM4";
	baud = 921600;

	serial::Serial my_serial(port, baud, serial::Timeout::simpleTimeout(1000));

	if (my_serial.isOpen())
		std::cout << " �ø��� ���� �۵� ��" << std::endl;
	else
		std::cout << " �ø��� �ȵſ�" << std::endl;

	//���� ����ó���ϱ�(�ð��Ǹ�) ������ ����ϳ�
	if (my_serial.available())
	{
		for (auto i = 0; i < 4096; ++i)
		{
			my_serial.read(&byte, 1);
			Calculators->CalCulate(byte);
		}
	}
}