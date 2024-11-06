#include "CSV.h"
#include <fstream>
#include <string>
#include "serial/serial.h"
#include "Core.h"




CSV::CSV()
{

}

CSV::~CSV()
{

}

void CSV::Instance()
{
	serial::Serial my_serial(Core::Cores->GetPortNumber(), Core::Cores->GetBaudrate(), serial::Timeout::simpleTimeout(1000));
	//시트 생성

}