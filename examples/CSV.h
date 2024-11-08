#pragma once
#include <iostream>
#include <vector>
#include "OpenXLSX.hpp"

using namespace OpenXLSX;
using namespace std;

class CSV
{
public:
	CSV();
	~CSV();

	void Instance(const int _Size);
protected:

private:
	void SetSaveDataSize(const int _Size)
	{
		this->DataSize = _Size;
	}
	void CreateExcel();
	void SerialWrite();
	void SaveToExcel();

	int DataSize = 0;
	int CellCount = 1; //나중에 시간넣으면 0으로 바꿔야함
	int LineCount = 2;

	std::string ReadLineData;

	XLDocument doc;
	XLWorksheet wks;

	XLStyleIndex cellFormatIndex;
	XLStyleIndex fontIndex;
	XLStyleIndex newFontIndex;
	XLStyleIndex newCellFormatIndex;

	std::vector<std::string> Cell
		= { "A","B","C","D","E", "F","G","H","I","J","K","L","M","N" };
	std::vector<std::string> CellName
		= { "Time","State","Detect","Power","AlertCount","Position","Drop","L_Floor","R_Floor","X","Y","Breath Data","Breath","Heart Data","Heart" };

	
};
