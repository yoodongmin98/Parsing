#pragma once

#include <iostream>




enum class CompanyData
{
	NONE,
	IQDATA,
	TI,
	TIMESTAMP,
	CSV,
};


class ParsingClass
{
	friend class Core;
public:
	ParsingClass();
	~ParsingClass();
	void ParsingStart();


	inline void SetCompanyData(CompanyData _Datas)
	{
		if (_Datas == CompanyData::NONE)
		{
			std::cout << "올바른 입력이 아닙니다" << std::endl;
		}
		this->Datas = _Datas;
	}
	inline const CompanyData GetCompanyData()
	{
		return Datas;
	}
private:
	CompanyData Datas = CompanyData::NONE;
	std::shared_ptr<class MyImGui> MyImGuis = nullptr;
	std::shared_ptr<class TI> Texas = nullptr;
	std::shared_ptr<class TimeStamp> UART_TimeStamp = nullptr;
	std::shared_ptr<class CSV> CSVFile = nullptr;
};
