#include "ProcessInfo.h"
#include "stdio.h"
#include "tlhelp32.h"
#include <string.h>
#include <tchar.h>
#include "psapi.h"
#include <vector>
#include <iostream>

#define PAGE_SIZE 0x1000
typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;

const char* architrecutreNames[] = { "No recognize","X86","X64" };

ProcessInfo::ProcessInfo()
{
	this->processBaseAddress = 0;
	this->entryPointAddress = 0;
	this->processHandle = NULL;
	memset(&(this->processBaseInformation), 0, sizeof(PROCESS_BASIC_INFORMATION));
	memset(&(this->processBaseInformation64), 0, sizeof(PROCESS_BASIC_INFORMATION_WOW64));
}

ProcessInfo::~ProcessInfo()
{

}

ProcessInfo& ProcessInfo::getInstance()
{
	static ProcessInfo processInfo;
	return processInfo;
}

/// <summary>
/// Recognize architecture using kernel32 function.
/// </summary>
/// <returns></returns>
int ProcessInfo::recognizeArchitecture()
{
	BOOL bIsWow64 = FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
	if (fnIsWow64Process != NULL) {
		if (!fnIsWow64Process(this->processHandle, &bIsWow64)) {
			return -1;
		}
	}
	if (bIsWow64) {
		return this->X86Arch;
	}
	else {
		return this->X64Arch;
	}
}

/// <summary>
/// Main function who getting base information. Its work at 64-bit architecture.
/// </summary>
/// <returns></returns>
bool ProcessInfo::loadProcessBaseInformation64()
{
	SIZE_T read_bytes = 0;
	BYTE headersBuf[PAGE_SIZE];

	this->processBaseAddress = this->getProcessBaseAddress(this->processHandle);
	if (this->processBaseAddress == NULL){
		printf("[ERROR] Get process base address!\n");
		return false;
	}

	printf("Process base address: 0x%I64X.\n", (uintptr_t)this->processBaseAddress);
	if (!ReadProcessMemory(this->processHandle, (LPCVOID)this->processBaseAddress, headersBuf, sizeof(headersBuf), &read_bytes) || read_bytes != sizeof(headersBuf)){
		printf("[ERROR] Read process memory failed.\n");
		return false;
	}

	IMAGE_NT_HEADERS* inh = get_nt_hrds32(headersBuf);
	if (inh == NULL) {
		printf("[ERROR] Get ntheaders32.\n");
		return false;
	}

	IMAGE_OPTIONAL_HEADER optHeader = inh->OptionalHeader;
	this->entryPointAddress = optHeader.AddressOfEntryPoint + (DWORD_PTR)this->processBaseAddress;
	printf("Entry point address : 0x%I64X.\n", (uintptr_t)this->entryPointAddress);
	return true;
}

/// <summary>
/// Get process base address.
/// </summary>
/// <param name="processHandle"></param>
/// <returns></returns>
DWORD_PTR ProcessInfo::getProcessBaseAddress(HANDLE processHandle)
{
	DWORD_PTR baseAddress = 0;
	HMODULE* moduleArray = NULL;
	LPBYTE moduleArrayBytes = NULL;
	DWORD bytesRequired = 0;

	if (!processHandle) {
		return NULL;
	}
	if (!EnumProcessModules(processHandle, NULL, 0, &bytesRequired)) {
		return NULL;
	}
	if (!bytesRequired) {
		return NULL;
	}

	moduleArrayBytes = (LPBYTE)LocalAlloc(LPTR, bytesRequired);
	if (!moduleArrayBytes) {
		return NULL;
	}
	
	unsigned int moduleCount;
	moduleCount = bytesRequired / sizeof(HMODULE);
	moduleArray = (HMODULE*)moduleArrayBytes;

	if (EnumProcessModules(processHandle, moduleArray, bytesRequired, &bytesRequired)){
		baseAddress = (DWORD_PTR)moduleArray[0];
		return baseAddress;
	}
	LocalFree(moduleArrayBytes);
	CloseHandle(processHandle);
	return NULL;
}

/// <summary>
/// Process handle setter.
/// </summary>
/// <param name="processHandle"></param>
void ProcessInfo::setProcessHandle(HANDLE processHandle)
{
	this->processHandle = processHandle;
}

/// <summary>
/// Get architecture as string.
/// </summary>
/// <returns></returns>
std::string ProcessInfo::getArchitectureString()
{
	int architecture = this->recognizeArchitecture();
	int archIndex = (architecture > 0) ? architecture : 0 ;
	return std::string(architrecutreNames[archIndex]);
}

/// <summary>
/// Process handle getter.
/// </summary>
/// <returns></returns>
HANDLE ProcessInfo::getProcessHandle()
{
	return this->processHandle;
}

/// <summary>
/// Process base address getter.
/// </summary>
/// <returns></returns>
LPVOID ProcessInfo::getProcessBaseAddress()
{
	return (LPVOID)this->processBaseAddress;
}

/// <summary>
/// Entry point address getter.
/// </summary>
/// <returns></returns>
DWORD_PTR ProcessInfo::getEntryPointAddress()
{
	return this->entryPointAddress;
}



/// <summary>
/// Not used at the moment.
/// </summary>
/// <returns></returns>
bool ProcessInfo::loadProcessBaseInformation()
{
	memset(&this->processBaseInformation, 0, sizeof(PROCESS_BASIC_INFORMATION_WOW64));

	if (NtQueryInformationProcess(this->processHandle, ProcessBasicInformation, &this->processBaseInformation, sizeof(PROCESS_BASIC_INFORMATION_WOW64), NULL) != 0)
	{
		printf("[ERROR] NtQueryInformationProcess failed\n");
		return false;
	}
	printf("pb : %p \n", this->processBaseInformation.PebBaseAddress);

	SIZE_T read_bytes = 0;
	if (!ReadProcessMemory(this->processHandle, (BYTE*)this->processBaseInformation.PebBaseAddress + 8, &this->processBaseAddress, sizeof(this->processBaseAddress), &read_bytes)
		|| read_bytes != sizeof(this->processBaseAddress)
		)
	{
		printf("[ERROR] Cannot read from PEB - incompatibile target!\n");
		return false;
	}
	printf("PEB = %P\n", (LPVOID)this->processBaseAddress);


	BYTE headersBuf[PAGE_SIZE];

	if (!ReadProcessMemory(this->processHandle, (LPCVOID)this->processBaseAddress, headersBuf, sizeof(headersBuf), &read_bytes) || read_bytes != sizeof(headersBuf))
	{
		printf("[ERROR] read process memory failed");
		return false;
	}


	IMAGE_NT_HEADERS* inh = get_nt_hrds32(headersBuf);

	if (inh == NULL)
		return false;

	IMAGE_OPTIONAL_HEADER optHeader = inh->OptionalHeader;
	this->entryPointAddress = optHeader.AddressOfEntryPoint + (DWORD)this->getProcessBaseAddress();

	printf("EP = 0x%x\n", this->entryPointAddress);

	return true;
}