#include "ProcessLoader.h"
#include <psapi.h>
#include <wchar.h>
#include "stdio.h"
#include "ntddk.h"
#include "pe_hdrs_helper.h"
#include "Utils.h"

using namespace std;

#define MAX_PROCESSES_ARRAY_SIZE 2048
#define PAGE_SIZE 0x1000


bool getProcessNameByHandle(HANDLE hProcess, LPWSTR nameBuf, SIZE_T nameMax);
bool isSearchedProcess(DWORD processID, LPWSTR searchedName);




ProcessLoader::ProcessLoader()
{
	this->processHandle = NULL;
}

ProcessLoader::~ProcessLoader()
{

}

ProcessLoader& ProcessLoader::getInstance()
{
	static ProcessLoader processLoader;
	return processLoader;
}



std::vector<ProcData_Typedef> ProcessLoader::getProcessList()
{
	DWORD processes[MAX_PROCESSES_ARRAY_SIZE];
	DWORD cbNeeded, cProcesses;

	vector<ProcData_Typedef> processList;

	if (!EnumProcesses(processes, sizeof(processes), &cbNeeded)) {
		return processList;
	}

	cProcesses = cbNeeded / sizeof(DWORD);
	for (int i = 0; i < cProcesses; i++)
	{
		if (processes[i] == 0) {
			continue;
		}

		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processes[i]);
		if (hProcess == NULL) {
			continue;
		}

		WCHAR szProcessName[MAX_PATH];
		if (getProcessNameByHandle(hProcess, szProcessName, MAX_PATH)) {
			ProcData_Typedef proc;
			proc.processName = Utils::convertWStringToString(wstring(szProcessName));
			proc.processPID = processes[i];
			processList.push_back(proc);
		}
		CloseHandle(hProcess);
	}

	return processList;
}

HANDLE ProcessLoader::loadProcessByPID(DWORD processPID)
{
	DWORD processes[MAX_PROCESSES_ARRAY_SIZE];
	DWORD cbNeeded, cProcesses;
	if (!EnumProcesses(processes, sizeof(processes), &cbNeeded)) {
		return NULL;
	}
	cProcesses = cbNeeded / sizeof(DWORD);
	for (int i = 0; i < cProcesses; i++) {
		if (processes[i] != 0) {
			if (processPID == processes[i])
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processes[i]);
				WCHAR szProcessName[MAX_PATH];
				if (getProcessNameByHandle(hProcess, szProcessName, MAX_PATH)) {
					this->processName = Utils::convertWStringToString(wstring(szProcessName));
					printf("process opened! \nprocess name: %s \nPID: %d \nprocess handle: %d \n", this->processName, processPID, this->processHandle);
					return hProcess;
				}
			}
		}
	}
	return NULL;
}


bool isSearchedProcess(DWORD processID, LPWSTR searchedName)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess == NULL) return false;

	WCHAR szProcessName[MAX_PATH];
	if (getProcessNameByHandle(hProcess, szProcessName, MAX_PATH)) {
		if (wcsstr(szProcessName, searchedName) != NULL) {
			printf("%S  (PID: %u)\n", szProcessName, processID);
			CloseHandle(hProcess);
			return true;
		}
	}
	CloseHandle(hProcess);
	return false;
}

bool getProcessNameByHandle(HANDLE hProcess, LPWSTR nameBuf, SIZE_T nameMax)
{
	HMODULE hMod;
	DWORD cbNeeded;

	if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
		GetModuleBaseName(hProcess, hMod, nameBuf, nameMax);
		return true;
	}
	return false;
}


void ProcessLoader::setProcessHandle(HANDLE processHandle)
{
	this->processHandle = processHandle;
}

HANDLE ProcessLoader::getProcessHandle()
{
	return this->processHandle;
}

string ProcessLoader::getProcessName()
{
	return this->processName;
}
