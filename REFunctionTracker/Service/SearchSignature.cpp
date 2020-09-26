#include "SearchSignature.h"
#include "stdio.h"
#include <string>
#include <vector>
#include <algorithm>


#define MAX_INSTRUCTIONS (500)
#define BUFFER_SIZE 8000


SearchSignature::SearchSignature()
{
	this->processHandle = NULL;
}

SearchSignature::SearchSignature(const SearchSignature&)
{
	this->processHandle = NULL;
}

/* 
 * Pobieranie instrukcji po ilosci oraz mnemonice od adresu
 */
int SearchSignature::getByInstruction(LPCVOID startAddress, unsigned long long startIndex, unsigned long long instructionCount, _DecodedInst instructions[],std::string mnemonic)
{

	BYTE buffer[BUFFER_SIZE];
	SIZE_T readedBytes;

	_DecodeResult res;
	_DecodeType dt = Decode64Bits;
	_DecodedInst instructionTemp[MAX_INSTRUCTIONS];


	unsigned int decodedInstructionsCount = 0;
	unsigned long long currentOffset = 0;
	unsigned long long currentInstructionCount = 0;
	unsigned long long correctInstructionIndex = 0;


	while (ReadProcessMemory(this->processHandle, (LPCVOID)((DWORD_PTR)startAddress + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes) != false)
	{
		decodedInstructionsCount = 0;
		res = distorm_decode64((DWORD_PTR)startAddress + currentOffset, (const unsigned char*)buffer, readedBytes, dt, instructionTemp, MAX_INSTRUCTIONS, &decodedInstructionsCount);
		if (res == DECRES_INPUTERR)
		{
			printf("Input error, halting!");
			return false;
		}

		currentOffset = (unsigned long)(instructionTemp[decodedInstructionsCount - 1].offset - ((DWORD_PTR)startAddress));

		for (int i = 0; i < decodedInstructionsCount; i++)
		{
			if (instructionTemp[i].mnemonic.length > 0)
			{
				if ((char)instructionTemp[i].mnemonic.p[0] == mnemonic.at(0))
				{
					if (std::strcmp(mnemonic.c_str(), (char*)instructionTemp[i].mnemonic.p) == 0)
					{

						if (correctInstructionIndex >= startIndex)
						{
							if (currentInstructionCount >= instructionCount)
							{
								break;
							}
							_DecodedInst instruction;
							memcpy(&instruction, &instructionTemp[i], sizeof(_DecodedInst));
							instructions[currentInstructionCount] = instruction;
							currentInstructionCount++;
						}
						correctInstructionIndex++;
					}
				}
			}
		}
		if (currentInstructionCount >= instructionCount)
		{
			break;
		}
	}


	return currentInstructionCount;
}


/*
 * Pobieranie maksymalnej ilosc instrukcji po mnemonice.
 */
unsigned long long SearchSignature::getMaxByInstruction(LPCVOID startAddress, std::string mnemonic)
{

	BYTE buffer[BUFFER_SIZE];
	SIZE_T readedBytes;

	_DecodeResult res;
	_DecodeType dt = Decode64Bits;
	_DecodedInst instructionTemp[MAX_INSTRUCTIONS];


	unsigned int decodedInstructionsCount = 0;
	unsigned long long currentOffset = 0;
	unsigned long long currentInstructionCount = 0;



	while (ReadProcessMemory(this->processHandle, (LPCVOID)((DWORD_PTR)startAddress + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes) != false)
	{
		decodedInstructionsCount = 0;
		res = distorm_decode64((DWORD_PTR)startAddress + currentOffset, (const unsigned char*)buffer, readedBytes, dt, instructionTemp, MAX_INSTRUCTIONS, &decodedInstructionsCount);
		if (res == DECRES_INPUTERR)
		{
			printf("Input error, halting!");
			return false;
		}

		currentOffset = (unsigned long)(instructionTemp[decodedInstructionsCount - 1].offset - ((DWORD_PTR)startAddress));

		for (int i = 0; i < decodedInstructionsCount; i++)
		{
			if (instructionTemp[i].mnemonic.length > 0)
			{
				if ((char)instructionTemp[i].mnemonic.p[0] == mnemonic.at(0))
				{
					if (std::strcmp(mnemonic.c_str(), (char*)instructionTemp[i].mnemonic.p) == 0)
					{
						currentInstructionCount++;
					}
				}
			}
		}
	}


	return currentInstructionCount;
}

/*
 * Pobranie instrukcji z adresu
*/
int SearchSignature::getInstructionByAddress(LPCVOID startAddress, _DecodedInst* instruction)
{
	BYTE buffer[BUFFER_SIZE];
	SIZE_T readedBytes;

	_DecodeResult res;
	_DecodeType dt = Decode64Bits;
	_DecodedInst instructionTemp[MAX_INSTRUCTIONS];

	unsigned int decodedInstructionsCount = 0;
	unsigned long long currentOffset = 0;
	unsigned long long currentInstructionCount = 0;

	while (ReadProcessMemory(this->processHandle, (LPCVOID)((DWORD_PTR)startAddress + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes) != false)
	{
		decodedInstructionsCount = 0;
		res = distorm_decode64((DWORD_PTR)startAddress + currentOffset, (const unsigned char*)buffer, readedBytes, dt, instructionTemp, MAX_INSTRUCTIONS, &decodedInstructionsCount);
		if (res == DECRES_INPUTERR)
		{
			printf("Input error, halting!");
			return false;
		}

		currentOffset = (unsigned long)(instructionTemp[decodedInstructionsCount - 1].offset - ((DWORD_PTR)startAddress));

		if (instructionTemp[0].offset == (unsigned long long) startAddress)
		{
			_DecodedInst newInstruction;
			memcpy(instruction, &instructionTemp[0], sizeof(_DecodedInst));
			currentInstructionCount = 1;
			break;
		}
		else
		{
			currentInstructionCount = -1;
			break;
		}

	}


	return currentInstructionCount;
}

/*
 * Pobranie instrukcji od adresu
 */
int SearchSignature::getByOperand(LPCVOID startAddress, unsigned long long startIndex, unsigned long long instructionCount, _DecodedInst instructions[], std::string mnemonic)
{

	BYTE buffer[BUFFER_SIZE];
	SIZE_T readedBytes;

	_DecodeResult res;
	_DecodeType dt = Decode64Bits;
	_DecodedInst instructionTemp[MAX_INSTRUCTIONS];


	unsigned int decodedInstructionsCount = 0;
	unsigned long long currentOffset = 0;
	unsigned long long currentInstructionCount = 0;
	unsigned long long correctInstructionIndex = 0;

	char* searchOperads = (char*)mnemonic.c_str();
	char* decodeOperands;

	std::transform(searchOperads, searchOperads + std::strlen(searchOperads), searchOperads, static_cast<int(*)(int)>(std::toupper));
	


	while (ReadProcessMemory(this->processHandle, (LPCVOID)((DWORD_PTR)startAddress + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes) != false)
	{
		decodedInstructionsCount = 0;
		res = distorm_decode64((DWORD_PTR)startAddress + currentOffset, (const unsigned char*)buffer, readedBytes, dt, instructionTemp, MAX_INSTRUCTIONS, &decodedInstructionsCount);
		if (res == DECRES_INPUTERR)
		{
			printf("Input error, halting!");
			return 0;
		}

		currentOffset = (unsigned long)(instructionTemp[decodedInstructionsCount - 1].offset - ((DWORD_PTR)startAddress));

		
		for (int i = 0; i < decodedInstructionsCount; i++)
		{
			if (instructionTemp[i].operands.length > 0)
			{
				decodeOperands = (char* )instructionTemp[i].operands.p;
				std::transform(decodeOperands, decodeOperands + std::strlen(decodeOperands), decodeOperands, static_cast<int(*)(int)>(std::toupper));


				if ((char)searchOperads[0] == decodeOperands[0])
				{
					if (std::strcmp(searchOperads, decodeOperands) == 0)
					{
						if (correctInstructionIndex >= startIndex)
						{
							if (currentInstructionCount >= instructionCount)
							{
								break;
							}
							_DecodedInst instruction;
							memcpy(&instruction, &instructionTemp[i], sizeof(_DecodedInst));
							instructions[currentInstructionCount] = instruction;
							currentInstructionCount++;
						}
						correctInstructionIndex++;
					}
				}
			}
		}
		if (currentInstructionCount >= instructionCount)
		{
			break;
		}
	}


	return currentInstructionCount;
}

unsigned long long SearchSignature::getMaxByOperand(LPCVOID startAddress, std::string mnemonic)
{

	BYTE buffer[BUFFER_SIZE];
	SIZE_T readedBytes;

	_DecodeResult res;
	_DecodeType dt = Decode64Bits;
	_DecodedInst instructionTemp[MAX_INSTRUCTIONS];


	unsigned int decodedInstructionsCount = 0;
	unsigned long long currentOffset = 0;
	unsigned long long currentInstructionCount = 0;



	while (ReadProcessMemory(this->processHandle, (LPCVOID)((DWORD_PTR)startAddress + currentOffset), (LPVOID)buffer, sizeof(buffer), &readedBytes) != false)
	{
		decodedInstructionsCount = 0;
		res = distorm_decode64((DWORD_PTR)startAddress + currentOffset, (const unsigned char*)buffer, readedBytes, dt, instructionTemp, MAX_INSTRUCTIONS, &decodedInstructionsCount);
		if (res == DECRES_INPUTERR)
		{
			printf("Input error, halting!");
			return false;
		}

		currentOffset = (unsigned long)(instructionTemp[decodedInstructionsCount - 1].offset - ((DWORD_PTR)startAddress));

		for (int i = 0; i < decodedInstructionsCount; i++)
		{
			if (instructionTemp[i].operands.length > 0)
			{
				if ((char)instructionTemp[i].operands.p[0] == mnemonic.at(0))
				{
					if (std::strcmp(mnemonic.c_str(), (char*)instructionTemp[i].operands.p) == 0)
					{
						currentInstructionCount++;
					}
				}
			}
		}
	}


	return currentInstructionCount;
}

void SearchSignature::setProcessHandle(HANDLE processHandle)
{
	this->processHandle = processHandle;
}

HANDLE SearchSignature::getProcessHandle()
{
	return this->processHandle;
}
