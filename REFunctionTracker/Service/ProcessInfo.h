#pragma once
#include <windows.h>
#include "ntddk.h"
#include "pe_hdrs_helper.h"
#include <string>

class ProcessInfo
{

	public:
		enum Architecture
		{
			X86Arch = 1,
			X64Arch = 2
		};
	
	public:
		static ProcessInfo& getInstance()
		{
			static ProcessInfo instance;
			return instance;
		}

	public:
		ProcessInfo();
		~ProcessInfo();
		void setProcessHandle(HANDLE processHandle);
		HANDLE getProcessHandle();
		bool loadProcessBaseInformation();
		int recognizeArchitecture();
		LPVOID getProcessBaseAddress();
		DWORD_PTR getEntryPointAddress();
		std::string getArchitectureString();
		bool loadProcessBaseInformation64();

	private:
		HANDLE processHandle;
		PROCESS_BASIC_INFORMATION processBaseInformation;
		PROCESS_BASIC_INFORMATION_WOW64 processBaseInformation64;
		DWORD_PTR processBaseAddress;
		DWORD_PTR entryPointAddress;
		
};

