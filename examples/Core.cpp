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
	//��Ʈ�� baudrate����
	port = "COM4";
	baud = 921600;

	serial::Serial my_serial(port, baud, serial::Timeout::simpleTimeout(1000));


	if (my_serial.available())
	{
		//���⼭ ������ �κи� ��󳻴� �۾� �ʿ�(�Ľ� �ʿ�)
		//���� �����͸� ���ð�� ����������ũ�⸸ŭ for�� �������
		//������� DataSize�����ϸ��
		for (auto i = 0; i < DataSize; ++i)
		{
			my_serial.read(&byte, 1);
			Calculators->CalCulate(byte);
		}
	}
}