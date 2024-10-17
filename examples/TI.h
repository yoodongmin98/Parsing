#pragma once
#include <iostream>


class TI
{
public:
	TI();
	~TI();

	void Instance();
protected:
private:
	uint8_t byte = 0;
	unsigned int FullDataSize = 0;
	unsigned int HeaderSize = 0;
	unsigned int TaleSize = 0;
	serial::Serial my_serial;
	void FindHeader();
	void PrintHeaderData();
};