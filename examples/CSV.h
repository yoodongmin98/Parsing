#pragma once
#include <iostream>
#include <vector>

class CSV
{
public:
	CSV();
	~CSV();

	void Instance();
protected:

private:
	std::vector<std::string> Cell
		= { "A","B","C","D","E", "F","G","H","I","J","K","L","M","N" };
	std::vector<std::string> CellName
		= { "Time","Detect","Power","AlertCount","Position","Drop","L_Floor","R_Floor","X","Y","Breath Data","Breath","Heart Data","Heart" };
};
