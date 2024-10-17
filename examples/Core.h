#pragma once
#include <string>
#include <iostream>

class ParsingClass;
class Core
{
public:
	static Core* Cores;
	Core();
	~Core();
	
	void Start();

	inline const std::string GetPortNumber()
	{
		return port;
	}
	inline const unsigned int GetBaudrate()
	{
		return baud;
	}
protected:

private:
	std::string port;
	unsigned long baud = 0;
	std::shared_ptr<class ParsingClass> Classes = nullptr;
};