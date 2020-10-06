#pragma once
#include <vector>
#include <Windows.h>
#include "AssemblerInstruction.h"
#include "distorm.h"


class Disassembler
{

public:
	Disassembler();
	~Disassembler();

public:
	static Disassembler& getInstance()
	{
		static Disassembler disassembler;
		return disassembler;
	}

public: 
	enum
	{
		DISSASSEMBLER_ERROR,
		DISSASSEMBLER_SUCESS,
	};

public:
	enum
	{
		X86Arch = 1,
		X64Arch = 2
	};


public:
	int dissassembly(DWORD_PTR codeOffset, const unsigned char* code, int codeLen,int architecture,std::vector<AssemblerInstruction*>&instructions);


};

