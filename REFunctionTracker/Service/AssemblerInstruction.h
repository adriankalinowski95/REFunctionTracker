#pragma once
#include <string>
#include <distorm.h>

class AssemblerInstruction
{
public:
	AssemblerInstruction(int architecture,_DecodedInst* decodedInst);
	AssemblerInstruction();
	~AssemblerInstruction();

public:
	enum Architecture
	{
		X86Arch = 1,
		X64Arch = 2
	};

public:
	void lazyInit(int architecture, _DecodedInst* decodedInst);
	int getArchitecture();
	std::string getMnemonic();
	std::string getOperands();
	std::string getInstructionHex();
	std::string getOffsetString();
	unsigned int getSize();
	unsigned __int64 getOffset();
	_DecodedInst* getDecodedInst();
	int getOperandsLength();
	int getInstructionLength();
	
private:
	_DecodedInst* decodedInst;
	int architecture; /*Architecture*/
};

