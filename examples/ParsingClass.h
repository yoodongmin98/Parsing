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
			return;
		this->Datas = _Datas;
	}
	inline const CompanyData GetCompanyData()
	{
		return Datas;
	}
private:
	CompanyData Datas = CompanyData::NONE;
	//class는 항상 내부 전방선언으로 사용하는걸 권장합니다.(컴파일 타임)
	std::shared_ptr<class MyImGui> MyImGuis = nullptr;
	std::shared_ptr<class TI> Texas = nullptr;
	std::shared_ptr<class TimeStamp> UART_TimeStamp = nullptr;
	std::shared_ptr<class CSV> CSVFile = nullptr;
};
