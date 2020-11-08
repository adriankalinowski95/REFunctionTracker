#pragma once
#include <windows.h>
#include "ntddk.h"
#include "pe_hdrs_helper.h"
#include <string>

class ProcessInfo
{

	public:
		enum Architecture{
			X86Arch = 1,
			X64Arch = 2
		};

	public:
		ProcessInfo();
		static ProcessInfo& getInstance();
		~ProcessInfo();

	public:
		HANDLE getProcessHandle();
		LPVOID getProcessBaseAddress();
		DWORD_PTR getEntryPointAddress();
		std::string getArchitectureString();

	public:
		int recognizeArchitecture();
		bool loadProcessBaseInformation64();
		void setProcessHandle(HANDLE processHandle);
		bool loadProcessBaseInformation();	//Not used!

	private:
		DWORD_PTR getProcessBaseAddress(HANDLE processHandle);

	private:
		HANDLE processHandle;
		PROCESS_BASIC_INFORMATION processBaseInformation;
		PROCESS_BASIC_INFORMATION_WOW64 processBaseInformation64;
		DWORD_PTR processBaseAddress;
		DWORD_PTR entryPointAddress;
		
};

