#include "ProcessLoader.h"
#include <psapi.h>
#include <wchar.h>
#include "stdio.h"
#include "ntddk.h"
#include "pe_hdrs_helper.h"

using namespace std;

#define MAX_PROCESSES_ARRAY_SIZE 2048
#define PAGE_SIZE 0x1000


bool getProcessNameByHandle(HANDLE hProcess, LPWSTR nameBuf, SIZE_T nameMax);
bool isSearchedProcess(DWORD processID, LPWSTR searchedName);

ProcessLoader::ProcessLoader()
{
	//this->processName = NULL;
	this->processHandle = NULL;
}

ProcessLoader::~ProcessLoader()
{

}

bool ProcessLoader::getProcessList(vector<ProcessStruct>* processList)
{
	DWORD processes[MAX_PROCESSES_ARRAY_SIZE];
	DWORD cbNeeded, cProcesses;
	if (!EnumProcesses(processes, sizeof(processes), &cbNeeded)) {
		return false;
	}
	cProcesses = cbNeeded / sizeof(DWORD);
	for (int i = 0; i < cProcesses; i++) {
		if (processes[i] != 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processes[i]);

			if (hProcess != NULL)
			{
				WCHAR szProcessName[MAX_PATH];
				if (getProcessNameByHandle(hProcess, szProcessName, MAX_PATH)) {
					struct ProcessStruct proc;
					wcscpy_s(proc.processName, szProcessName);
					proc.processPID = processes[i];
					processList->push_back(proc);
				}

			}
			CloseHandle(hProcess);
		}
	}
	return true;
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
					this->processHandle = hProcess;
					wcscpy_s(this->processName, szProcessName);
					//injectedProcessName = szProcessName;
					printf("process opened! \nprocess name: %S \nPID: %d \nprocess handle: %d \n", this->processName, processPID, this->processHandle);
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

LPWSTR ProcessLoader::getProcessName()
{
	return this->processName;
}
