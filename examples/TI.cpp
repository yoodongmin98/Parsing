#include "TI.h"
#include "serial/serial.h"
#include "Core.h"
#include <vector>
#include <algorithm>

TI::TI()
{

}

TI::~TI()
{

}


void TI::Instance()
{
	while (true)
	{
		FindHeader();

	}
}


void TI::FindHeader()
{
	while (true)
	{
		//얘 위치 바꿔야함
		serial::Serial my_serial(Core::Cores->GetPortNumber(), Core::Cores->GetBaudrate(), serial::Timeout::simpleTimeout(1000));
	
		std::vector<unsigned int> Header;
		my_serial.read(&byte, 1);
		Header.push_back(byte);
		if (Header.size() > 8)//MagicNumberSize
		{
			//투 포인터 알고리즘
			int FirstPoint = 0, LastPoint = 8;
			sort(Header.begin() + FirstPoint, Header.begin() + LastPoint);
			for ()
		}
	}
	
}
void TI::PrintHeaderData()
{

}
