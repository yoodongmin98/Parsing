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

	//�� ũ�� �� �ʱ� �ؽ�Ʈ ����
	for (auto i = 0; i < Cell.size(); ++i)
	{
		//�� ũ�� �����ϴ°�
		float cellwidth = CellName[i].size() + 10;
		wks.column(Cell[i]).setWidth(cellwidth);
		std::string Cells = Cell[i] + "1";
		wks.cell(Cells).value().set(CellName[i]);

		//�׳� ��Ʈ ���� �ٲٴ°�
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
		//FV Data Protocol�� N �Ǵ� Y�� �����ϹǷ� �� �ն����� �˻�

		std::string Times = Time::Times->GetLocalTime();
		std::string Cells = Cell[0] + to_string(LineCount);
		wks.cell(Cells).value().set(Times); //���� �ð������� �ٲ����
		if (ReadLineData[0] == 'N' || ReadLineData[0] == 'Y')
		{
			for (auto i = 0; i < ReadLineData.size() - 1; ++i)
			{
				//FV ��� �����ʹ� ��ĭ�� �ټ� �����ϰ������Ƿ� ��ĭ�� �ѱ�
				if (ReadLineData[i] == ' ') continue;
				//�ݷ��� �������� �����͸� ����
				//�ݷ��� �ƴҶ��� string�� �ϳ��� ���ؼ� ���ڿ��� ����� �ݷ��� ������ ���� ���ο� �� �����͸� Write��
				if (ReadLineData[i] != ':') {
					Data += ReadLineData[i];
				}
				else {
					//A���� ...���ĺ��� ���� Cell�� ���鼭 A1 B1 C1������� �����͸� �����
					std::string Cells = Cell[CellCount] + to_string(LineCount);
					//���
					wks.cell(Cells).value().set(Data);
					//���ͳ��θ� �������� CellCount�� ++�Ͽ� B,C,D�� �ѱ��.
					CellCount++;
					Data.clear();
				}
			}
			//�� �ڿ��� �ݷ�(:)�� ��µ��������Ƿ� size��ŭ ���� �� �� ���� �����͸� ����ϰ� �ʿ� �������� �ʱ�ȭ�Ѵ�.
			std::string Cells = Cell[CellCount] + to_string(LineCount);
			wks.cell(Cells).value().set(Data);
			Data.clear();
			WriteCount++; LineCount++; CellCount = 1;
			std::cout << "\n���� ������ �� : " << WriteCount << flush;
		}
	}
}


void CSV::SaveToExcel()
{
	doc.save();
	doc.close();
}