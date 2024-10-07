#pragma once


#define DataSize 4096 //DataSize¼³Á¤

class Calculator;
class Core
{
public:
	Core();
	~Core();
	
	void Start(int argc, char** argv);
protected:

private:
	uint8_t byte = 0;
	std::string port;
	unsigned long baud = 0;
	std::shared_ptr<Calculator> Calculators = nullptr;
};