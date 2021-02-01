#pragma once
#include <vector>
//#include "Assembler.h"
#include "AssemblerInstruction.h"
#include "Service/ProcessInfo.h"


class ProcessInstructionReader
{
public:
	ProcessInstructionReader();
	static ProcessInstructionReader& getInstance();
	~ProcessInstructionReader();

public:
	enum
	{
		PROCESS_INSTRUCTION_READER_ERROR = -1,
		PROCESS_INSTRUCTION_READER_SUCESS = 1,
		PROCESS_INSTRUCTION_READER_NO_READED_INSTRUCTIONS = 0
		
	};
//wektory do: 
	//Konkretna inktrukcja po indexie
	//X intrukcji od adresu
	//


//mainform 
public:
	int getInstructionByIndex(unsigned long long startAddress, unsigned long long startIndex, int instructionCount, std::vector<AssemblerInstruction*>& instructions);
	int getInstructionByAddress(unsigned long long address, int instructionCount, std::vector<AssemblerInstruction*>& instructions);
	bool getOneInstByAddress(unsigned long long address,AssemblerInstruction& asmInst);
	bool getOneInstByIndex(unsigned long long index, AssemblerInstruction& asmInst);
	long getInstructionIndex(unsigned long long startAddress, unsigned long long instructionAddress);
	unsigned long long getProcessInstructionCount(unsigned long long startAddress);
	unsigned long long getProcessSize(unsigned long long startAddress);
	void changeByBP(unsigned long long startAddress,unsigned long bytes,uint8_t *arrayPtr,uint8_t arraySize);
	void setBpInVec(std::vector<AssemblerInstruction*>& vec);
//search signature form
public:
	int getByInstruction(unsigned long long startAddress, unsigned long long startIndex, unsigned long long instructionCount, std::vector<AssemblerInstruction*>& instructions,std::string mnemonic);
	unsigned long getMaxByInstruction(unsigned long long startAddress,std::string mnemonic);
	int getInstructionByAddress(unsigned long long startAddress, AssemblerInstruction** instruction);
	int getByOperand(unsigned long long  startAddress, unsigned long long startIndex, unsigned long long instructionCount, std::vector<AssemblerInstruction*>& instructions, std::string mnemonic);
	unsigned long long getMaxByOperand(unsigned long long startAddress, std::string mnemonic);
	

private:
	int getProcessArchitecture();

};

