#pragma once
#include <thread>
#include <windows.h>
#include <vector>
#include "AssemblerInstruction.h"

typedef struct{
	unsigned long long address;
	unsigned long index;
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
	std::vector<BreakPoint_Typedef>& getCurrentBreakPoins();
	DEBUGGER_STATUS removeBeakPoint(unsigned long long address);
	bool isBreakPointWithIndex(unsigned long long index);
	bool isBreakPointWithAddress(unsigned long long address);

private:
	std::thread* debugThread;
	void enterDebugLoop(DWORD processPID);
	std::vector<BreakPoint_Typedef> breakPointsVector;
	bool isDebugThread;
	

};

void setBreakPointsForVec(std::vector<AssemblerInstruction*>& instructions);
