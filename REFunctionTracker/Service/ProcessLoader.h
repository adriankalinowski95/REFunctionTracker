#pragma once
#include <windows.h>
#include <vector>
#include <string>



typedef struct {

	std::string processName;
	DWORD processPID;
}ProcData_Typedef;


class ProcessLoader
{

public:
	ProcessLoader();
	static ProcessLoader& getInstance();
	~ProcessLoader();

public:
	std::vector<ProcData_Typedef> getProcessList();
	HANDLE loadProcessByPID(DWORD processPID);
	void setProcessHandle(HANDLE processHandle);
	HANDLE getProcessHandle();
	std::string getProcessName();



private:
	HANDLE processHandle;
	std::string processName;

};

