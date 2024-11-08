#include "Core.h"
#include "CSV.h"
#include "OpenXLSX.hpp"
#include "serial/serial.h"
#include <string>


using namespace OpenXLSX;
using namespace std;



CSV::CSV()
{

}

CSV::~CSV()
{

}

void CSV::Instance(const int _Size)
{
	SetSaveDataSize(_Size);
	CreateExcel();
	SerialWrite();
}


void CSV::CreateExcel()
{
	doc.create("FV_Data.xlsx");
	wks = doc.workbook().worksheet("Sheet1");

	XLCellFormats& cellFormats = doc.styles().cellFormats();
	XLFonts& fonts = doc.styles().fonts();

	//셀 크기 및 초기 텍스트 설정
	for (auto i = 0; i < Cell.size(); ++i)
	{
		float cellwidth = CellName[i].size() + 10;
		wks.column(Cell[i]).setWidth(cellwidth);
		std::string Cells = Cell[i] + "1";
		wks.cell(Cells).value().set(CellName[i]);

		//그냥 폰트 굵게 바꾸는거
		cellFormatIndex = wks.cell(Cells).cellFormat();
		fontIndex = cellFormats[cellFormatIndex].fontIndex();
		newFontIndex = fonts.create(fonts[fontIndex]);
		fonts[newFontIndex].setBold();

		newCellFormatIndex = cellFormats.create(cellFormats[cellFormatIndex]);
		cellFormats[newCellFormatIndex].setFontIndex(newFontIndex);
		wks.cell(Cells).setCellFormat(newCellFormatIndex);
	}
}


void CSV::SerialWrite()
{
	serial::Serial my_serial(Core::Cores->GetPortNumber(), Core::Cores->GetBaudrate(), serial::Timeout::simpleTimeout(1000));
	std::string Data = "";
	while (DataSize--)
	{
		//Serial Data
		ReadLineData = my_serial.readline();
		if (ReadLineData[0] == 'N' || ReadLineData[0] == 'Y')
		{
			for (auto i = 0; i < ReadLineData.size() - 1; ++i)
			{
				if (ReadLineData[i] == ' ') continue;

				if (ReadLineData[i] != ':') {
					Data += ReadLineData[i];
				}
				else {
					std::string Cells = Cell[CellCount] + to_string(LineCount);
					wks.cell(Cells).value().set(Data);
					CellCount++;
					Data.clear();
				}
			}
			std::string Cells = Cell[CellCount] + to_string(LineCount);
			wks.cell(Cells).value().set(Data);
			Data.clear();
			LineCount++;
			CellCount = 1; 	 //나중에 시간넣으면 0으로 바꿔야함
		}
	}
}


void CSV::SaveToExcel()
{
	doc.save();
	doc.close();
}