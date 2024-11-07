#include "Core.h"
#include "CSV.h"
#include "OpenXLSX.hpp"
#include "serial/serial.h"
#include <fstream>
#include <string>


using namespace OpenXLSX;
using namespace std;



CSV::CSV()
{

}

CSV::~CSV()
{

}

void CSV::Instance()
{
	serial::Serial my_serial(Core::Cores->GetPortNumber(), Core::Cores->GetBaudrate(), serial::Timeout::simpleTimeout(1000));
	XLDocument doc;
	doc.create("FV_Data.xlsx");
	
	auto wks = doc.workbook().worksheet("Sheet1");

	XLCellFormats& cellFormats = doc.styles().cellFormats();
	XLFonts& fonts = doc.styles().fonts();

	XLStyleIndex cellFormatIndex;
	XLStyleIndex fontIndex;
	XLStyleIndex newFontIndex;
	XLStyleIndex newCellFormatIndex;


	//셀 크기 및 초기 텍스트 설정
	for (auto i = 0; i < Cell.size(); ++i)
	{
		float cellwidth = CellName[i].size()+10;
		wks.column(Cell[i]).setWidth(cellwidth);
		std::string Cells = Cell[i] + "1";
		wks.cell(Cells).value().set(CellName[i]);


		cellFormatIndex = wks.cell(Cells).cellFormat();
		fontIndex = cellFormats[cellFormatIndex].fontIndex();
		newFontIndex = fonts.create(fonts[fontIndex]);
		fonts[newFontIndex].setBold();

		newCellFormatIndex = cellFormats.create(cellFormats[cellFormatIndex]);
		cellFormats[newCellFormatIndex].setFontIndex(newFontIndex);  
		wks.cell(Cells).setCellFormat(newCellFormatIndex);
	}

	int CellCount = 1; //나중에 시간넣으면 0으로 바꿔야함
	int LineCount = 2;
	std::string Data="";
	while (true)
	{
		//Serial Data
		std::string ReadData = my_serial.readline();
		if (ReadData[0] == 'N' || ReadData[0] == 'Y')
		{
			for (auto i = 0; i < ReadData.size() - 1; ++i)
			{
				if (ReadData[i] == ' ') continue;

				if (ReadData[i] != ':'){
					Data += ReadData[i];
				}
				else
				{
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
			//입력받아서 while문 안에 넣기
			if (LineCount == 50)
				break;
		}
	}
	
	doc.save();
	doc.close();
}