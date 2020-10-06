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
	this->processPID = 0;
}

ProcessLoader::~ProcessLoader()
{

}

ProcessLoader& ProcessLoader::getInstance()
{
	static ProcessLoader processLoader;
	return processLoader;
}



std::vector<ProcData> ProcessLoader::getProcessList()
{
	DWORD processes[MAX_PROCESSES_ARRAY_SIZE];
	DWORD cbNeeded, cProcesses;
	cbNeeded = 23;
	vector<ProcData> processList;

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
			ProcData proc;
			proc.processName = Utils::convertWStringToString(wstring(szProcessName));
			proc.processPID = processes[i];
			processList.push_back(proc);
		}
		CloseHandle(hProcess);
	}

	return processList;
}

std::string ProcessLoader::getProcessListAsJSON() {
	ProcessLoader* procLoadInst = &(ProcessLoader::getInstance());
	std::vector<ProcData> vecProcData = procLoadInst->getProcessList();
	return Utils::serializeToJSON<std::vector<ProcData>>(vecProcData, "processList").c_str();
}

ProcData ProcessLoader::jsonToProcData(std::string json)
{
	return Utils::serializeToObject<ProcData>(json);
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
		if (processes[i] == 0) {
			continue;
		}
		if (processPID == processes[i])
		{
			continue;
		}
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processes[i]);
		WCHAR szProcessName[MAX_PATH];
		if (getProcessNameByHandle(hProcess, szProcessName, MAX_PATH)) {
			this->processName = Utils::convertWStringToString(wstring(szProcessName));
			this->processHandle = hProcess;
			this->processPID = processPID;
			printf("process opened! \nprocess name: %s \nPID: %d\n", this->processName.c_str(), processPID);
			return hProcess;
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
