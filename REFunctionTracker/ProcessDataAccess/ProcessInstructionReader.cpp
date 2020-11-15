#include "ProcessDataAccess/ProcessInstructionReader.h"
#include "Disassembler.h"
#include <algorithm>
#define BUFFER_SIZE 8000

ProcessInstructionReader::ProcessInstructionReader()
{

}

ProcessInstructionReader::~ProcessInstructionReader()
{

}

ProcessInstructionReader& ProcessInstructionReader::getInstance()
{
	static ProcessInstructionReader processInstructionReader;
	return processInstructionReader;
}

int ProcessInstructionReader::getInstructionByIndex(unsigned long long startAddress, unsigned long long startIndex, int instructionCount, std::vector<AssemblerInstruction*>& instructions)
{
	ProcessInfo* processInformation = &(ProcessInfo::getInstance());
	Disassembler* disassembler = &(Disassembler::getInstance());
	int architecture = this->getProcessArchitecture();

	std::vector<AssemblerInstruction*> tempInstructions;

	unsigned char buffer[BUFFER_SIZE];
	unsigned long long currentOffset = 0;
	unsigned long long instructionsCount = 0;
	SIZE_T readedBytes;
	int status = 0;

	if (processInformation->getProcessHandle() == NULL) {
		return PROCESS_INSTRUCTION_READER_ERROR;
	}

	while (ReadProcessMemory(processInformation->getProcessHandle(), (LPCVOID)((DWORD_PTR)startAddress + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes) != false)
	{
		status = disassembler->dissassembly((DWORD_PTR)startAddress + currentOffset, (const unsigned char*)buffer, readedBytes, architecture,tempInstructions);
		
		if (status == Disassembler::DISSASSEMBLER_ERROR) {
			return PROCESS_INSTRUCTION_READER_ERROR;
		}

		currentOffset = (unsigned long)(tempInstructions.at(tempInstructions.size() - 1)->getOffset() - ((DWORD_PTR)startAddress));

		if (startIndex >= instructionsCount && startIndex <= (instructionsCount + tempInstructions.size()))
		{
			for (int i = 0; i < tempInstructions.size(); i++)
			{
				if ((i + instructionsCount) >= (startIndex) && (i + instructionsCount) < (startIndex + instructionCount))
				{
					_DecodedInst* newDecodedInst = new _DecodedInst;
					memcpy(newDecodedInst, tempInstructions.at(i)->getDecodedInst(), sizeof(_DecodedInst));
					AssemblerInstruction* newInstruction = new AssemblerInstruction(architecture, newDecodedInst);
					instructions.push_back(newInstruction);
				}
			}
		}

		instructionsCount += tempInstructions.size();
		while (!tempInstructions.empty()) {
			delete tempInstructions.back();
			tempInstructions.pop_back();
		}

		if (instructions.size() >= instructionCount){
			break;
		}
	}

	return instructions.size() > 0 ? PROCESS_INSTRUCTION_READER_SUCESS : PROCESS_INSTRUCTION_READER_NO_READED_INSTRUCTIONS;
}

int ProcessInstructionReader::getInstructionByAddress(unsigned long long address, int instructionCount, std::vector<AssemblerInstruction*>& instructions)
{
	ProcessInfo* processInformation = &(ProcessInfo::getInstance());
	Disassembler* disassembler = &(Disassembler::getInstance());
	int architecture = this->getProcessArchitecture();

	std::vector<AssemblerInstruction*> tempInstructions;

	unsigned char buffer[BUFFER_SIZE];
	unsigned long long currentOffset = 0;
	unsigned long long instructionsCount = 0;
	SIZE_T readedBytes;
	int status = 0;

	if (processInformation->getProcessHandle() == NULL)
		return PROCESS_INSTRUCTION_READER_ERROR;



	while (ReadProcessMemory(processInformation->getProcessHandle(), (LPCVOID)((DWORD_PTR)address + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes) != false)
	{
		status = disassembler->dissassembly((DWORD_PTR)address + currentOffset, (const unsigned char*)buffer, readedBytes, architecture, tempInstructions);

		if (status == Disassembler::DISSASSEMBLER_ERROR) {
			return PROCESS_INSTRUCTION_READER_ERROR;
		}

		currentOffset = (unsigned long)(tempInstructions.at(tempInstructions.size() - 1)->getOffset() - ((DWORD_PTR)address));

		if (instructions.size() < instructionCount)
		{
			for (int i = 0; i < tempInstructions.size(); i++)
			{
				if (instructions.size() < instructionCount)
				{
					_DecodedInst* newDecodedInst = new _DecodedInst;
					memcpy(newDecodedInst, tempInstructions.at(i)->getDecodedInst(), sizeof(_DecodedInst));
					AssemblerInstruction* newInstruction = new AssemblerInstruction(architecture, newDecodedInst);
					instructions.push_back(newInstruction);
				} else {
					break;
				}
			}
		}
		instructionsCount += tempInstructions.size();
		while (!tempInstructions.empty()) {
			delete tempInstructions.back();
			tempInstructions.pop_back();
		}

		if (instructions.size() >= instructionCount){
			break;
		}
	}

	return instructions.size() > 0 ? PROCESS_INSTRUCTION_READER_SUCESS : PROCESS_INSTRUCTION_READER_ERROR;
}

unsigned long long ProcessInstructionReader::getProcessInstructionCount(unsigned long long startAddress)
{
	ProcessInfo* processInformation = &(ProcessInfo::getInstance());
	Disassembler* disassembler = &(Disassembler::getInstance());
	int architecture = this->getProcessArchitecture();

	std::vector<AssemblerInstruction*> tempInstructions;

	unsigned char buffer[BUFFER_SIZE];
	unsigned long long currentOffset = 0;
	unsigned long long instructionsCount = 0;
	SIZE_T readedBytes;
	int status = 0;

	if (processInformation->getProcessHandle() == NULL) {
		return PROCESS_INSTRUCTION_READER_ERROR;
	}

	while (ReadProcessMemory(processInformation->getProcessHandle(), (LPCVOID)((DWORD_PTR)startAddress + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes) != false)
	{
		status = disassembler->dissassembly((DWORD_PTR)startAddress + currentOffset, (const unsigned char*)buffer, readedBytes, architecture, tempInstructions);

		if (status == Disassembler::DISSASSEMBLER_ERROR)
			return PROCESS_INSTRUCTION_READER_ERROR;

		currentOffset = (unsigned long)(tempInstructions.at(tempInstructions.size() - 1)->getOffset() - ((DWORD_PTR)startAddress));
		instructionsCount += tempInstructions.size();

		while (!tempInstructions.empty()) {
			delete tempInstructions.back();
			tempInstructions.pop_back();
		}
	}

	return instructionsCount;
}

unsigned long long ProcessInstructionReader::getProcessSize(unsigned long long startAddress)
{
	ProcessInfo* processInformation = &(ProcessInfo::getInstance());
	unsigned long long currentOffset = 0;
	BYTE buffer[BUFFER_SIZE];
	SIZE_T readedBytes;
	while (ReadProcessMemory(processInformation->getProcessHandle(), (LPCVOID)((DWORD_PTR)startAddress + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes) != false){
			currentOffset += readedBytes;
	}
	return currentOffset;
}

unsigned long ProcessInstructionReader::getInstructionIndex(unsigned long long startAddress, unsigned long long instructionAddress)
{
	ProcessInfo* processInformation = &(ProcessInfo::getInstance());
	Disassembler* disassembler = &(Disassembler::getInstance());
	int architecture = this->getProcessArchitecture();

	std::vector<AssemblerInstruction*> tempInstructions;

	unsigned char buffer[BUFFER_SIZE];
	unsigned long long currentOffset = 0;
	unsigned long long instructionsCount = 0;
	SIZE_T readedBytes;
	int status = 0;

	if (processInformation->getProcessHandle() == NULL)
		return PROCESS_INSTRUCTION_READER_ERROR;



	while (ReadProcessMemory(processInformation->getProcessHandle(), (LPCVOID)((DWORD_PTR)startAddress + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes) != false)
	{
		status = disassembler->dissassembly((DWORD_PTR)startAddress + currentOffset, (const unsigned char*)buffer, readedBytes, architecture, tempInstructions);

		if (status == Disassembler::DISSASSEMBLER_ERROR)
			return PROCESS_INSTRUCTION_READER_ERROR;

		currentOffset = (unsigned long)(tempInstructions.at(tempInstructions.size() - 1)->getOffset() - ((DWORD_PTR)startAddress));


		if (((DWORD_PTR)instructionAddress >= tempInstructions.at(0)->getOffset() && ((DWORD_PTR)instructionAddress <= tempInstructions.at(tempInstructions.size() - 1)->getOffset())))
		{
			for (int i = 0; i < tempInstructions.size(); i++)
			{
				if (tempInstructions.at(i)->getOffset() == (DWORD_PTR)instructionAddress)
				{
					return  i + instructionsCount;
				}
			}
		}

		instructionsCount += tempInstructions.size();
		while (!tempInstructions.empty()) {
			delete tempInstructions.back();
			tempInstructions.pop_back();
		}
	}

	return PROCESS_INSTRUCTION_READER_ERROR;

}

int ProcessInstructionReader::getByInstruction(unsigned long long startAddress, unsigned long long startIndex, unsigned long long instructionCount,
												std::vector<AssemblerInstruction*>& instructions, std::string mnemonic)
{
	std::transform(mnemonic.begin(), mnemonic.end(), mnemonic.begin(), std::toupper);

	ProcessInfo* processInformation = &(ProcessInfo::getInstance());
	Disassembler* disassembler = &(Disassembler::getInstance());
	int architecture = this->getProcessArchitecture();
	std::vector<AssemblerInstruction*> tempInstructions;
	unsigned char buffer[BUFFER_SIZE];
	unsigned long long currentOffset = 0;
	unsigned long long instructionsCount = 0;
	SIZE_T readedBytes;
	int status = 0;

	if (processInformation->getProcessHandle() == NULL)
		return PROCESS_INSTRUCTION_READER_ERROR;

	while (ReadProcessMemory(processInformation->getProcessHandle(), (LPCVOID)((DWORD_PTR)startAddress + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes) != false)
	{
		status = disassembler->dissassembly((DWORD_PTR)startAddress + currentOffset, (const unsigned char*)buffer, readedBytes, architecture, tempInstructions);

		if (status == Disassembler::DISSASSEMBLER_ERROR)
			return PROCESS_INSTRUCTION_READER_ERROR;

		currentOffset = (unsigned long)(tempInstructions.at(tempInstructions.size() - 1)->getOffset() - ((DWORD_PTR)startAddress));

		for (int i = 0; i < tempInstructions.size(); i++)
		{
			if (tempInstructions.at(i)->getMnemonic().c_str()[0] == mnemonic.c_str()[0])
			{
				if (tempInstructions.at(i)->getMnemonic().compare(mnemonic.c_str()) == 0)
				{
					if (instructionsCount >= startIndex)
					{
						if (instructions.size() < instructionCount)
						{
							_DecodedInst* newDecodedInst = new _DecodedInst;
							memcpy(newDecodedInst, tempInstructions.at(i)->getDecodedInst(), sizeof(_DecodedInst));
							AssemblerInstruction* newInstruction = new AssemblerInstruction(architecture, newDecodedInst);
							instructions.push_back(newInstruction);
						}
						else
							break;
					}
					instructionsCount++;
					
				}
			}
		}

		while (!tempInstructions.empty()) {
			delete tempInstructions.back();
			tempInstructions.pop_back();
		}

		if (instructions.size() >= instructionCount)
		{
			break;
		}
	}

	return instructions.size() > 0 ? PROCESS_INSTRUCTION_READER_SUCESS : PROCESS_INSTRUCTION_READER_ERROR;
}

unsigned long ProcessInstructionReader::getMaxByInstruction(unsigned long long startAddress, std::string mnemonic)
{
	std::transform(mnemonic.begin(), mnemonic.end(), mnemonic.begin(), std::toupper);

	ProcessInfo* processInformation = &(ProcessInfo::getInstance());
	Disassembler* disassembler = &(Disassembler::getInstance());

	int architecture = this->getProcessArchitecture();
	std::vector<AssemblerInstruction*> tempInstructions;

	unsigned char buffer[BUFFER_SIZE];
	unsigned long long currentOffset = 0;
	unsigned long long instructionsCount = 0;
	SIZE_T readedBytes;
	int status = 0;

	if (processInformation->getProcessHandle() == NULL)
		return PROCESS_INSTRUCTION_READER_ERROR;

	while (ReadProcessMemory(processInformation->getProcessHandle(), (LPCVOID)((DWORD_PTR)startAddress + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes) != false)
	{
		status = disassembler->dissassembly((DWORD_PTR)startAddress + currentOffset, (const unsigned char*)buffer, readedBytes, architecture, tempInstructions);

		if (status == Disassembler::DISSASSEMBLER_ERROR)
			return PROCESS_INSTRUCTION_READER_ERROR;

		currentOffset = (unsigned long)(tempInstructions.at(tempInstructions.size() - 1)->getOffset() - ((DWORD_PTR)startAddress));

		for (int i = 0; i < tempInstructions.size(); i++)
		{
			if (tempInstructions.at(i)->getMnemonic().c_str()[0] == mnemonic.c_str()[0])
			{
				if (tempInstructions.at(i)->getMnemonic().compare(mnemonic.c_str()) == 0)
				{
					instructionsCount++;
				}
			}
		}

		while (!tempInstructions.empty()) {
			delete tempInstructions.back();
			tempInstructions.pop_back();
		}
	}

	return instructionsCount;
}

int ProcessInstructionReader::getInstructionByAddress(unsigned long long startAddress, AssemblerInstruction** instruction)
{

	ProcessInfo* processInformation = &(ProcessInfo::getInstance());
	Disassembler* disassembler = &(Disassembler::getInstance());

	int architecture = this->getProcessArchitecture();
	std::vector<AssemblerInstruction*> tempInstructions;

	unsigned char buffer[BUFFER_SIZE];
	unsigned long long currentOffset = 0;
	unsigned long long instructionsCount = 0;
	SIZE_T readedBytes;
	int status = 0;

	if (processInformation->getProcessHandle() == NULL)
		return PROCESS_INSTRUCTION_READER_ERROR;

	ReadProcessMemory(processInformation->getProcessHandle(), (LPCVOID)((DWORD_PTR)startAddress + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes);
	status = disassembler->dissassembly((DWORD_PTR)startAddress + currentOffset, (const unsigned char*)buffer, readedBytes, architecture, tempInstructions);

	if (status == Disassembler::DISSASSEMBLER_ERROR)
			return PROCESS_INSTRUCTION_READER_ERROR;

	if (tempInstructions.at(0)->getOffset() == (unsigned long long) startAddress)
	{
		_DecodedInst* newDecodedInst = new _DecodedInst;
		memcpy(newDecodedInst, tempInstructions.at(0)->getDecodedInst(), sizeof(_DecodedInst));
		AssemblerInstruction* newInstruction = new AssemblerInstruction(architecture, newDecodedInst);
		*instruction = newInstruction;
		return PROCESS_INSTRUCTION_READER_SUCESS;
	}

	while (!tempInstructions.empty()) {
		delete tempInstructions.back();
		tempInstructions.pop_back();
	}
	

	return PROCESS_INSTRUCTION_READER_ERROR;
}

int ProcessInstructionReader::getByOperand(unsigned long long  startAddress, unsigned long long startIndex, unsigned long long instructionCount,
										std::vector<AssemblerInstruction*>& instructions, std::string mnemonic)
{
	std::transform(mnemonic.begin(), mnemonic.end(), mnemonic.begin(), std::toupper);

	ProcessInfo* processInformation = &(ProcessInfo::getInstance());
	Disassembler* disassembler = &(Disassembler::getInstance());
	int architecture = this->getProcessArchitecture();
	std::vector<AssemblerInstruction*> tempInstructions;
	unsigned char buffer[BUFFER_SIZE];
	unsigned long long currentOffset = 0;
	unsigned long long instructionsCount = 0;
	SIZE_T readedBytes;
	int status = 0;

	if (processInformation->getProcessHandle() == NULL)
		return PROCESS_INSTRUCTION_READER_ERROR;

	std::string tempString;

	while (ReadProcessMemory(processInformation->getProcessHandle(), (LPCVOID)((DWORD_PTR)startAddress + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes) != false)
	{
		status = disassembler->dissassembly((DWORD_PTR)startAddress + currentOffset, (const unsigned char*)buffer, readedBytes, architecture, tempInstructions);

		if (status == Disassembler::DISSASSEMBLER_ERROR)
			return PROCESS_INSTRUCTION_READER_ERROR;

		currentOffset = (unsigned long)(tempInstructions.at(tempInstructions.size() - 1)->getOffset() - ((DWORD_PTR)startAddress));

		for (int i = 0; i < tempInstructions.size(); i++)
		{
			if (tempInstructions.at(i)->getOperandsLength() > 0)
			{
				tempString = tempInstructions.at(i)->getOperands();
				std::transform(tempString.begin(), tempString.end(), tempString.begin(), std::toupper);

				if (tempString.c_str()[0] == mnemonic.c_str()[0])
				{
					if (tempString.compare(mnemonic.c_str()) == 0)
					{
						
						if (instructionsCount >= startIndex)
						{
							if (instructions.size() < instructionCount)
							{
								_DecodedInst* newDecodedInst = new _DecodedInst;
								memcpy(newDecodedInst, tempInstructions.at(i)->getDecodedInst(), sizeof(_DecodedInst));
								AssemblerInstruction* newInstruction = new AssemblerInstruction(architecture, newDecodedInst);
								instructions.push_back(newInstruction);
							}
							else
								break;
						}
						instructionsCount++;

					}
				}
			}
		}
		while (!tempInstructions.empty()) {
			delete tempInstructions.back();
			tempInstructions.pop_back();
		}

		if (instructions.size() >= instructionCount)
		{
			break;
		}
	}

	return instructions.size() > 0 ? PROCESS_INSTRUCTION_READER_SUCESS : PROCESS_INSTRUCTION_READER_ERROR;
}

unsigned long long ProcessInstructionReader::getMaxByOperand(unsigned long long startAddress, std::string mnemonic)
{
	std::transform(mnemonic.begin(), mnemonic.end(), mnemonic.begin(), std::toupper);

	ProcessInfo* processInformation = &(ProcessInfo::getInstance());
	Disassembler* disassembler = &(Disassembler::getInstance());

	int architecture = this->getProcessArchitecture();
	std::vector<AssemblerInstruction*> tempInstructions;

	unsigned char buffer[BUFFER_SIZE];
	unsigned long long currentOffset = 0;
	unsigned long long instructionsCount = 0;
	SIZE_T readedBytes;
	int status = 0;

	if (processInformation->getProcessHandle() == NULL)
		return PROCESS_INSTRUCTION_READER_ERROR;
	std::string tempString;
	while (ReadProcessMemory(processInformation->getProcessHandle(), (LPCVOID)((DWORD_PTR)startAddress + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes) != false)
	{
		status = disassembler->dissassembly((DWORD_PTR)startAddress + currentOffset, (const unsigned char*)buffer, readedBytes, architecture, tempInstructions);

		if (status == Disassembler::DISSASSEMBLER_ERROR)
			return PROCESS_INSTRUCTION_READER_ERROR;

		currentOffset = (unsigned long)(tempInstructions.at(tempInstructions.size() - 1)->getOffset() - ((DWORD_PTR)startAddress));

		for (int i = 0; i < tempInstructions.size(); i++)
		{
			if (tempInstructions.at(i)->getOperands().length() > 0)
			{
				tempString = tempInstructions.at(i)->getOperands();

				std::transform(tempString.begin(), tempString.end(), tempString.begin(), std::toupper);
				if (tempString.c_str()[0] == mnemonic.c_str()[0])
				{
					if (tempString.compare(mnemonic.c_str()) == 0)
					{
						instructionsCount++;
					}
				}
			}
		}

		while (!tempInstructions.empty()) {
			delete tempInstructions.back();
			tempInstructions.pop_back();
		}
	}

	return instructionsCount;
}

int ProcessInstructionReader::getProcessArchitecture()
{
	
	ProcessInfo *processInformation = &(ProcessInfo::getInstance());

	if (processInformation->getProcessHandle() == NULL)
		return 0;

	int architecture = processInformation->recognizeArchitecture();
	return architecture;
}
