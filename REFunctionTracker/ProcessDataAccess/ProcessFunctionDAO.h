#pragma once
#include <Windows.h>

class ProcessFunctionDAO
{

public:
	static ProcessFunctionDAO& getInstance()
	{
		static ProcessFunctionDAO processFunctionDAO;
		return processFunctionDAO;
	}

public:
	ProcessFunctionDAO();
	~ProcessFunctionDAO();
	ProcessFunctionDAO(const ProcessFunctionDAO&);
	void setProcessHandle(HANDLE processHandle);
	HANDLE getProcessHandle();
	bool isProcessFunctionExist(unsigned long long functionAddress);
	//ProcessFunction* getProcessFunctionByAddress(unsigned long long functionAddress);
	//bool loadInstructions(ProcessFunction* processFunction);
	//bool recognizeConvection(ProcessFunction* processFunction);

private:
	HANDLE processHandle;
};

