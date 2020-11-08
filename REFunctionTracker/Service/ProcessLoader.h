#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

struct ProcData{
	std::string processName;
	DWORD processPID;

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("processName", processName)
				,cereal::make_nvp("processPID", processPID));
	}
};

class ProcessLoader
{

public:
	ProcessLoader();
	static ProcessLoader& getInstance();
	~ProcessLoader();

public:
	std::vector<ProcData> getProcessList();
	std::string getProcessListAsJSON();
	ProcData jsonToProcData(std::string json);
	HANDLE loadProcessByPID(DWORD processPID);
	void setProcessHandle(HANDLE processHandle);
	HANDLE getProcessHandle();
	std::string getProcessName();

private:
	HANDLE processHandle;
	std::string processName;
	DWORD processPID;

};
