#pragma once
#include <thread>
#include <windows.h>
#include <vector>
#include "AssemblerInstruction.h"

typedef struct{
	unsigned long long address;
	AssemblerInstruction* prevInst;
}BreakPoint_Typedef;

class Debugger
{
public:
	enum DEBUGGER_STATUS {
		FAIL = -1,
		SUCCESS = 1
	};
public:
	Debugger();
	static Debugger& getInstance();
	~Debugger();

public:
	DEBUGGER_STATUS startDebugThread();
	DEBUGGER_STATUS setBreakPoint(unsigned long long address);

private:
	std::thread* debugThread;
	void enterDebugLoop(HANDLE processHandle);
	std::vector<BreakPoint_Typedef> breakPointsVector;
};