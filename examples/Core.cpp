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
	//포트와 baudrate설정
	port = "COM4";
	baud = 921600;

	serial::Serial my_serial(port, baud, serial::Timeout::simpleTimeout(1000));


	if (my_serial.available())
	{
		//여기서 데이터 부분만 골라내는 작업 필요(파싱 필요)
		//순수 데이터만 나올경우 원본데이터크기만큼 for문 돌리면됨
		//헤더에서 DataSize설정하면됨
		for (auto i = 0; i < DataSize; ++i)
		{
			my_serial.read(&byte, 1);
			Calculators->CalCulate(byte);
		}
	}
}