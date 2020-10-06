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


const char* architrecutreNames[] = {"No recognize","X86","X64"};

DWORD_PTR GetProcessBaseAddress(HANDLE processHandle);


ProcessInfo::ProcessInfo()
{
	this->processBaseAddress = 0;
}
void ProcessInfo::setProcessHandle(HANDLE processHandle)
{
	this->processHandle = processHandle;
}
HANDLE ProcessInfo::getProcessHandle()
{
	return this->processHandle;
}
ProcessInfo::~ProcessInfo()
{

}


int ProcessInfo::recognizeArchitecture()
{	

	BOOL bIsWow64 = FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(this->processHandle, &bIsWow64))
			return -1;
	}
	if (bIsWow64)
		return this->X86Arch;
	else
		return this->X64Arch;
}


bool ProcessInfo::loadProcessBaseInformation64()
{

	SIZE_T read_bytes = 0;

	this->processBaseAddress = GetProcessBaseAddress(this->processHandle);

	if (this->processBaseAddress == NULL)
	{
		printf("[ERROR] process base address");
		return false;
	}
	printf("base address : 0x%I64X ", (uintptr_t)this->processBaseAddress);

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
	this->entryPointAddress = optHeader.AddressOfEntryPoint + (DWORD_PTR)this->getProcessBaseAddress();

	printf("entry point address : 0x%I64X ", (uintptr_t)this->entryPointAddress);

	return true;
}

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

DWORD_PTR GetProcessBaseAddress(HANDLE processHandle)
{
	DWORD_PTR   baseAddress = 0;
	HMODULE* moduleArray;
	LPBYTE      moduleArrayBytes;
	DWORD       bytesRequired;

	if (processHandle)
	{
		if (EnumProcessModules(processHandle, NULL, 0, &bytesRequired))
		{
			if (bytesRequired)
			{
				moduleArrayBytes = (LPBYTE)LocalAlloc(LPTR, bytesRequired);

				if (moduleArrayBytes)
				{
					unsigned int moduleCount;

					moduleCount = bytesRequired / sizeof(HMODULE);
					moduleArray = (HMODULE*)moduleArrayBytes;

					if (EnumProcessModules(processHandle, moduleArray, bytesRequired, &bytesRequired))
					{
						baseAddress = (DWORD_PTR)moduleArray[0];
						return baseAddress;
					}

					LocalFree(moduleArrayBytes);
				}
			}
		}

		CloseHandle(processHandle);
	}
	return 0;
}


std::string ProcessInfo::getArchitectureString()
{
	int architecture = this->recognizeArchitecture();
	if (architecture > 0)
		return std::string(architrecutreNames[architecture]);
	else
		return std::string(architrecutreNames[0]);
}


LPVOID ProcessInfo::getProcessBaseAddress()
{
	return (LPVOID)this->processBaseAddress;
}


DWORD_PTR ProcessInfo::getEntryPointAddress()
{
	return this->entryPointAddress;
}






