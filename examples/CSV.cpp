#include "Core.h"
#include "CSV.h"
#include "OpenXLSX.hpp"
#include "serial/serial.h"
#include <fstream>
#include <string>



using namespace OpenXLSX;



CSV::CSV()
{

}

CSV::~CSV()
{

}


void CSV::Instance()
{
	//serial::Serial my_serial(Core::Cores->GetPortNumber(), Core::Cores->GetBaudrate(), serial::Timeout::simpleTimeout(1000));
	//XLDocument doc;
	//doc.create("FV_Data.xlsx");

	//auto wks = doc.workbook().worksheet("Sheet1");

	////셀 크기 및 초기 텍스트 설정
	//for (auto i=0; i<Cell.size(); ++i)
	//{
	//	float cellwidth = CellName[i].size()+10;
	//	wks.column(Cell[i]).setWidth(cellwidth);
	//	Cell[i] += "1";
	//	wks.cell(Cell[i]).value().set(CellName[i]);
	//}
}