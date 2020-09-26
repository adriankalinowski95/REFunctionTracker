#pragma once
#include <Windows.h>
#include <string>
#include "distorm.h"

class SearchSignature
{
public:
	static SearchSignature& getInstance()
	{
		static SearchSignature searchSignature;
		return searchSignature;
	}

public:
	SearchSignature();
	SearchSignature(const SearchSignature&);
	void setProcessHandle(HANDLE processHandle);
	HANDLE getProcessHandle();
	int getByInstruction(LPCVOID startAddress,unsigned long long startIndex,unsigned long long  instructionCount, _DecodedInst instructions[],std::string mnemonic);
	unsigned long long getMaxByInstruction(LPCVOID startAddress, std::string mnemonic);
	int getInstructionByAddress(LPCVOID startAddress, _DecodedInst* instruction);
	int getByOperand(LPCVOID startAddress, unsigned long long startIndex, unsigned long long instructionCount, _DecodedInst instructions[], std::string mnemonic);
	unsigned long long getMaxByOperand(LPCVOID startAddress, std::string mnemonic);


private:
	HANDLE processHandle;

	

};

