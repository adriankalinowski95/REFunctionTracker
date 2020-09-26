#pragma once
#include <windows.h>
#include <vector>

struct ProcessStruct {

	WCHAR processName[MAX_PATH];
	DWORD processPID;
};


class ProcessLoader
{
public:
	static ProcessLoader& getInstance()
	{
		static ProcessLoader processLoader;
		return processLoader;
	}

public:
	ProcessLoader();
	~ProcessLoader();
	bool getProcessList(std::vector<ProcessStruct>* processList);
	HANDLE loadProcessByPID(DWORD processPID);
	void setProcessHandle(HANDLE processHandle);
	HANDLE getProcessHandle();
	LPWSTR getProcessName();



private:
	HANDLE processHandle;
	WCHAR processName[MAX_PATH];

};

