#pragma once

#include <iostream>

using namespace std;


enum class CompanyData
{
	NONE,
	TIHEADER,
};

class ParsingClass
{
public:
	void SetCompanyData(CompanyData _Datas)
	{
		this->Datas = _Datas;
	}
	void StartParsing()
	{
		if (Datas == CompanyData::NONE)
			return;
	}
	
private:
	CompanyData Datas;
};
