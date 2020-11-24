#include "Debugger.h"
#include <minwinbase.h>
#include <WinBase.h>
#include "ProcessInfo.h"
#include "ProcessLoader.h"
#include "ProcessInstructionReader.h"

Debugger::Debugger()
{

}

Debugger& Debugger::getInstance()
{
	static Debugger instance;
	return instance;
}

Debugger::~Debugger()
{

}


void Debugger::enterDebugLoop(DWORD processPID)
{
	BOOL status = DebugActiveProcess(processPID);
	DEBUG_EVENT debugEvent;
	while (1)
	{
		WaitForDebugEvent(&debugEvent, INFINITE);
		if (debugEvent.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {
			if (debugEvent.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT) {
				// @TODO ADD inst back.
				printf("Catch break point!");
			}
		}
		ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, DBG_CONTINUE);
	}
}

Debugger::DEBUGGER_STATUS Debugger::startDebugThread()
{
	ProcessInfo* processInfo = &(ProcessInfo::getInstance());
	ProcessLoader* processLoader = &(ProcessLoader::getInstance());
	if (this->isDebugThread) {
		return Debugger::DEBUGGER_STATUS::SUCCESS;
	}
	DWORD processPID = processLoader->getProcessPID();
	this->debugThread = new std::thread(&Debugger::enterDebugLoop,this, processLoader->getProcessPID());
	this->isDebugThread = true;

	return Debugger::DEBUGGER_STATUS::SUCCESS;
}

/* Dodac delete */
Debugger::DEBUGGER_STATUS Debugger::setBreakPoint(unsigned long long address) {

	ProcessInfo* processInfo = &(ProcessInfo::getInstance());
	ProcessInstructionReader* procInstReadInstance = &(ProcessInstructionReader::getInstance());
	HANDLE processHandle = processInfo->getProcessHandle();
	AssemblerInstruction* instruction;
	BYTE cInstruction = 0xCC;
	SIZE_T writtenBytes;
	int status;

	if (processHandle == NULL) {
		return Debugger::DEBUGGER_STATUS::FAIL;
	}
	status = procInstReadInstance->getInstructionByAddress((unsigned long long)address, &instruction);
	if (status != ProcessInstructionReader::PROCESS_INSTRUCTION_READER_SUCESS) {
		return Debugger::DEBUGGER_STATUS::FAIL;
	}
	
	BOOL writeStatus = WriteProcessMemory(processHandle, (void*)instruction->getOffset(), &cInstruction, 1, &writtenBytes);
	if (writtenBytes != 1) {
		return Debugger::DEBUGGER_STATUS::FAIL;
	}

	BreakPoint_Typedef breakPoint;
	breakPoint.address = address;
	breakPoint.prevInst = instruction;
	this->breakPointsVector.push_back(breakPoint);

	return Debugger::DEBUGGER_STATUS::SUCCESS;
}


