#include "ProcessFunctionDAO.h"
#include "ProcessInstructionReader.h"
#include "Service/SearchSignature.h"
#include "Service/ProcessInfo.h"
#include "Service/PEreader.h"
#include <string>

#define MINIMUM_TO_FUNCTION_ACCEPT 1
#define START_SEARCHING_INDEX 0
#define APPERANCE_BUFFOR 1
#define FUNCTION_ADDRESS_BUFFOR_SIZE 50
#define CALL_MNEMONIC "CALL"
#define RET_MNEMONIC "RET"
#define INSTRUCTION_PAGINATION 2000


ProcessFunctionDAO::ProcessFunctionDAO()
{
}

ProcessFunctionDAO::~ProcessFunctionDAO()
{
}

ProcessFunctionDAO::ProcessFunctionDAO(const ProcessFunctionDAO&)
{
}

void ProcessFunctionDAO::setProcessHandle(HANDLE processHandle)
{
	this->processHandle = processHandle;
}

HANDLE ProcessFunctionDAO::getProcessHandle()
{
	return this->processHandle;
}

bool ProcessFunctionDAO::isProcessFunctionExist(unsigned long long functionAddress)
{
	//SearchSignature* searchSignature = &(SearchSignature::getInstance());
	ProcessInfo* processInfo = &(ProcessInfo::getInstance());
	ProcessInstructionReader* pir = &(ProcessInstructionReader::getInstrance());
	std::vector<AssemblerInstruction*> instructions;

	//_DecodedInst apperanceInstructions[APPERANCE_BUFFOR];

	char instructionAddress[FUNCTION_ADDRESS_BUFFOR_SIZE];

	sprintf(instructionAddress, "0x%I64X", functionAddress);


	for (auto& c : instructionAddress)
	{
		c = tolower(c);
	}
	int appearanceCount = pir->getByOperand((unsigned long long )processInfo->getProcessBaseAddress(),
												START_SEARCHING_INDEX, MINIMUM_TO_FUNCTION_ACCEPT, instructions, instructionAddress);
	//int appearanceCount = searchSignature->getByOperand(processInfo->getProcessBaseAddress(), START_SEARCHING_INDEX,
	//	MINIMUM_TO_FUNCTION_ACCEPT, apperanceInstructions, instructionAddress);

	if (appearanceCount == 0)
	{
		return false;
	}
	else
		return true;
}

/*
* Dodac sprawdzanie w sytuacji gdy CALL/JMP jest do pointera.
*/
ProcessFunction* ProcessFunctionDAO::getProcessFunctionByAddress(unsigned long long functionAddress)
{

	if (isProcessFunctionExist(functionAddress) == false)
	{
		return NULL;
	}

	ProcessInfo* processInfo = &(ProcessInfo::getInstance());
	PEReader* processReader = &(PEReader::getInstance());
	
	unsigned long long readedProcesses = 0;
	unsigned long long currentInstruction = 0;
	unsigned long long endProcessAddress = 0;
	unsigned long long functionInstructionCount = 0;

	//_DecodedInst instructionBuffer[INSTRUCTION_PAGINATION];
	ProcessInstructionReader* pir = &(ProcessInstructionReader::getInstrance());
	std::vector<AssemblerInstruction*> instructions;
	
	do
	{
		//readedProcesses = processReader->getInstructionsByIndex((LPCVOID)functionAddress, instructionBuffer, currentInstruction, INSTRUCTION_PAGINATION);
		readedProcesses = pir->getInstructionByIndex(functionAddress, currentInstruction, INSTRUCTION_PAGINATION, instructions);
		for (int i = 0; i < instructions.size(); i++)
		{
			//if (strcmp((char*)instructionBuffer[i].mnemonic.p, RET_MNEMONIC) == 0)
			if (strcmp(instructions.at(i)->getMnemonic().c_str(), RET_MNEMONIC) == 0 )
			{
				functionInstructionCount = currentInstruction + i + 1;
			//	endProcessAddress = instructionBuffer[i].offset;
				endProcessAddress = instructions.at(i)->getOffset();
				printf("RET AT : 0x%I64X INSTRUCCTION CONUT %d \n", endProcessAddress, functionInstructionCount);
				break;
			}
		}
		if (endProcessAddress > 0)
		{
			break;
		}
		currentInstruction += INSTRUCTION_PAGINATION;

	}while(readedProcesses > 0);

	if (endProcessAddress == 0)
	{
		return NULL;
	}

	ProcessFunction* newProcessFunction = new ProcessFunction((unsigned long long)processInfo->getProcessBaseAddress(),
										  functionAddress - (unsigned long long)processInfo->getProcessBaseAddress(),
										  endProcessAddress - (unsigned long long)processInfo->getProcessBaseAddress(),
										  functionInstructionCount);


	if (loadInstructions(newProcessFunction) == false)
	{
		return NULL;
	}

	return newProcessFunction;
}

bool ProcessFunctionDAO::loadInstructions(ProcessFunction* processFunction)
{
	//PEReader* processReader = &(PEReader::getInstance());

	//int instructionCount = processFunction->getInstructionCount();

	//_DecodedInst* instructionBuffer = new _DecodedInst[instructionCount+1];
	
	
	//int readedInstructions = processReader->getInstructionsByIndex((LPCVOID)(processFunction->getBaseAddress() + processFunction->getOffsetStartAddress()) 
	//															, instructionBuffer, 0, instructionCount);

	//processFunction->instructions = instructionBuffer;

	ProcessInstructionReader* pir = &(ProcessInstructionReader::getInstrance());
	int instructionCount = processFunction->getInstructionCount();

	std::vector<AssemblerInstruction*>* instructions = new std::vector<AssemblerInstruction*>();
	processFunction->instructions = instructions;


	//int readedInstructions = processReader->getInstructionsByIndex((LPCVOID)(processFunction->getBaseAddress() + processFunction->getOffsetStartAddress()) 
	//													, instructionBuffer, 0, );

	int sucess = pir->getInstructionByIndex(processFunction->getBaseAddress() + processFunction->getOffsetStartAddress(),0, instructionCount,*instructions);

	if (processFunction->getInstructionCount() > 0 && sucess ==  ProcessInstructionReader::PROCESS_INSTRUCTION_READER_SUCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ProcessFunctionDAO::recognizeConvection(ProcessFunction* processFunction)
{
	return false;
}
