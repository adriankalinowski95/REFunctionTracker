#pragma once
#include <windows.h>
#include "ntddk.h"
#include "pe_hdrs_helper.h"
#include <string>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

struct ProcessBaseInfo
{
	unsigned long long baseAddress;
	unsigned long long entryPointAddress;
	unsigned long long entryPointIndex;
	unsigned long long instructionCount;

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("baseAddress", baseAddress),
			cereal::make_nvp("entryPointAddress", entryPointAddress),
			cereal::make_nvp("entryPointIndex", entryPointIndex),
			cereal::make_nvp("instructionCount", instructionCount));
	}

};

class ProcessInfo
{

	public:
		enum Architecture {
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
		std::string getProcessBaseInfoJSON();

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

