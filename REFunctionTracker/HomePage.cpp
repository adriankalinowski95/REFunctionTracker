#include "HomePage.h"
#include "ProcessLoader.h"
#include "ProcessInfo.h"
#include "ProcessDataAccess/ProcessInstructionReader.h"
#include "InfoMessage.h"
#include "Utils.h"

#include <fstream>
#include <cereal/archives/json.hpp>
#include <vector>


HomePage::HomePage()
{

}

HomePage::HomePage(RefPtr<Overlay>overlay) : overlay_(overlay)
{
	Ref<JSContext> context = overlay->view()->LockJSContext();
	SetJSContext(context.get());
	JSObject global = JSGlobalObject();
	global["OnProcessLoad"] = BindJSCallback(&HomePage::OnProcessLoad);
	loadDisAsmTable = global["loadDisAsmTable"];

	global["GetProcessBaseInfo"] = BindJSCallbackWithRetval(&HomePage::GetProcessBaseInfo);
	global["GetProcessInstructionByIndex"] = BindJSCallbackWithRetval(&HomePage::GetProcessInstructionByIndex);
	global["GetProcessInstructionByAddress"] = BindJSCallbackWithRetval(&HomePage::GetProcessInstructionByAddress);
	
}

JSValue HomePage::GetProcessBaseInfo(const JSObject& thisObject, const JSArgs& args)
{
	ProcessInfo* procInfoInst = &(ProcessInfo::getInstance());
	std::string procBaseInfo = procInfoInst->getProcessBaseInfoJSON();
	if (procBaseInfo.empty()) {
		return JSValue(false);
	}

	printf("Inst count %s \n", procBaseInfo.c_str());
	return JSValue(procBaseInfo.c_str());
}

JSValue HomePage::GetProcessInstructionByIndex(const JSObject& thisObject, const JSArgs& args) {
	if (args.size() != 2) {
		return JSValue(false);
	}
	int startIndex = args[0];
	int count = args[1];
	if (startIndex < 0 || count <= 0) {
		return JSValue(false);
	}

	ProcessInfo* procInfoInst = &(ProcessInfo::getInstance());
	if (procInfoInst->getProcessBaseAddress() <= 0) {
		return JSValue(false);
	}
	
	std::vector<AssemblerInstruction*> instructions;
	ProcessInstructionReader* procInstReader = &(ProcessInstructionReader::getInstance());
	procInstReader->getInstructionByIndex((unsigned long long)procInfoInst->getProcessBaseAddress(), (unsigned long long)startIndex,count, instructions);
	/* Sprawdzic czy to na pewno to. */
	if (instructions.size() < count) {
		//Doda� free
		while (!instructions.empty()) {
			delete instructions.back();
			instructions.pop_back();
		}
		long long newStartIndex = startIndex - (count - instructions.size());
		if (newStartIndex > 0) {
			procInstReader->getInstructionByIndex((unsigned long long)procInfoInst->getProcessBaseAddress(), (unsigned long long)newStartIndex, count, instructions);
		}
	}
	if (instructions.size() <= 0) {
		return JSValue(false);
	}
	
	std::vector<ASMInst> asmInstructions;
	for (int i = 0; i < instructions.size(); i++) {
		asmInstructions.push_back(instructions[i]->getStruct());
	}

	std::string strInstructions = Utils::serializeToJSON<std::vector<ASMInst>>(asmInstructions, "instructions");
	printf("Instructions: %s\n", strInstructions.c_str());

	return JSValue(strInstructions.c_str());
}


JSValue HomePage::GetProcessInstructionByAddress(const JSObject& thisObject, const JSArgs& args) {
	if (args.size() != 2) {
		return JSValue(false);
	}
	unsigned long long startAddress = args[0];
	int count = args[1];
	if (startAddress < 0 || count <= 0) {
		return JSValue(false);
	}

	ProcessInfo* procInfoInst = &(ProcessInfo::getInstance());
	if (procInfoInst->getProcessBaseAddress() <= 0) {
		return JSValue(false);
	}

	std::vector<AssemblerInstruction*> instructions;
	ProcessInstructionReader* procInstReader = &(ProcessInstructionReader::getInstance());
	procInstReader->getInstructionByAddress(startAddress, count, instructions);
	if (instructions.size() <= 0) {
		return JSValue(false);
	}

	std::vector<ASMInst> asmInstructions;
	for (int i = 0; i < instructions.size(); i++) {
		asmInstructions.push_back(instructions[i]->getStruct());
	}

	std::string strInstructions = Utils::serializeToJSON<std::vector<ASMInst>>(asmInstructions, "instructions");
	printf("Instructions: %s\n", strInstructions.c_str());

	return JSValue(strInstructions.c_str());
}


HomePage::~HomePage()
{

}

void HomePage::OnProcessLoad(const JSObject& obj, const JSArgs& args)
{
	ProcessLoader* procLoadInst = &(ProcessLoader::getInstance());
	ProcessInfo* procInfoInst = &(ProcessInfo::getInstance());
	bool status = false;
	procInfoInst->setProcessHandle(procLoadInst->getProcessHandle());
	status = procInfoInst->loadProcessBaseInformation64();
	if (!status) {
		loadDisAsmTable({ false });
		return;
	}
	loadDisAsmTable({ true });
}


