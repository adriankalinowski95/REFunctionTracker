#include "Debugger.h"
#include <minwinbase.h>
#include <WinBase.h>
#include "ProcessInfo.h"
#include "ProcessLoader.h"
#include "ProcessInstructionReader.h"
#include "Utils.h"

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
	ProcessInfo* processInfo = &(ProcessInfo::getInstance());
	PROCESS_INFORMATION_CLASS procClass;
	unsigned long long ripRegister;
	unsigned long long exceptionAddress;
	CONTEXT lcContext;
	HANDLE hThread;
	DEBUGGER_STATUS removeBreakPointStatus;
	while (1)
	{
		WaitForDebugEvent(&debugEvent, INFINITE);
		if (debugEvent.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {
			if (debugEvent.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT) {
				// @TODO ADD inst back.
				hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, debugEvent.dwThreadId);
				lcContext.ContextFlags = CONTEXT_ALL;
				GetThreadContext(hThread, &lcContext);
				ripRegister = lcContext.Rip;
				exceptionAddress = (unsigned long long)debugEvent.u.Exception.ExceptionRecord.ExceptionAddress;

				removeBreakPointStatus = this->removeBeakPoint(exceptionAddress);
				if (removeBreakPointStatus == DEBUGGER_STATUS::SUCCESS){
					printf("[SUCCESS] REMOVE BREAK POINT at: %llu", exceptionAddress);
					lcContext.Rip--;
					SetThreadContext(hThread, &lcContext);
				} else {
					printf("[ERROR] CANT REMOVE BREAK POINT.");
				}
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

Debugger::DEBUGGER_STATUS Debugger::removeBeakPoint(unsigned long long address) {
	ProcessInfo* processInfo = &(ProcessInfo::getInstance());
	if (processInfo->getProcessHandle() == NULL) {
		return Debugger::DEBUGGER_STATUS::FAIL;
	}
	SIZE_T writtenBytes;
	for (int i = 0; i < this->breakPointsVector.size(); i++) {
		if (this->breakPointsVector.at(i).address == address) {
			std::string instructionHex = this->breakPointsVector.at(i).prevInst->getInstructionHex();
			std::vector<char> instructionBytes = Utils::HexToBytes(instructionHex);
			if (instructionBytes.size() <= 0) {
				return Debugger::DEBUGGER_STATUS::FAIL;

			}
			uint8_t firstByte = instructionBytes.at(0);
			BOOL writeStatus = WriteProcessMemory(processInfo->getProcessHandle(),
				(void*)this->breakPointsVector.at(i).prevInst->getOffset(),
				&firstByte,
				1,
				&writtenBytes);
			
			if (writeStatus) {
				return Debugger::DEBUGGER_STATUS::SUCCESS;
			}
			else {
				return Debugger::DEBUGGER_STATUS::FAIL;
			}
		}
	}
	return Debugger::DEBUGGER_STATUS::FAIL;
}


