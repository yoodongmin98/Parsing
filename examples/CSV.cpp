#include "Core.h"
#include "CSV.h"
#include "OpenXLSX.hpp"
#include "serial/serial.h"
#include <string>
#include "Time.h"


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
	SaveToExcel();
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
		//셀 크기 변경하는거
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
		//FV Data Protocol이 N 또는 Y로 시작하므로 맨 앞라인을 검사

		std::string Times = Time::Times->GetLocalTime();
		std::string Cells = Cell[0] + to_string(LineCount);
		wks.cell(Cells).value().set(Times); //여기 시간나오게 바꿔야함
		if (ReadLineData[0] == 'N' || ReadLineData[0] == 'Y')
		{
			for (auto i = 0; i < ReadLineData.size() - 1; ++i)
			{
				//FV 출력 데이터는 빈칸을 다수 포함하고있으므로 빈칸은 넘김
				if (ReadLineData[i] == ' ') continue;
				//콜론을 기준으로 데이터를 나눔
				//콜론이 아닐때는 string을 하나씩 더해서 문자열을 만들고 콜론이 있으면 엑셀 라인에 그 데이터를 Write함
				if (ReadLineData[i] != ':') {
					Data += ReadLineData[i];
				}
				else {
					//A부터 ...알파벳을 담은 Cell을 돌면서 A1 B1 C1순서대로 데이터를 기록함
					std::string Cells = Cell[CellCount] + to_string(LineCount);
					//기록
					wks.cell(Cells).value().set(Data);
					//벡터내부를 돌기위해 CellCount를 ++하여 B,C,D로 넘긴다.
					CellCount++;
					Data.clear();
				}
			}
			//맨 뒤에는 콜론(:)이 출력되지않으므로 size만큼 전부 돈 후 남은 데이터를 기록하고 필요 변수들을 초기화한다.
			std::string Cells = Cell[CellCount] + to_string(LineCount);
			wks.cell(Cells).value().set(Data);
			Data.clear();
			WriteCount++; LineCount++; CellCount = 1;
			std::cout << "\n쓰인 데이터 수 : " << WriteCount << flush;
		}
	}
}


void CSV::SaveToExcel()
{
	doc.save();
	doc.close();
}