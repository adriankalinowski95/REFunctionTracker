#include "HomePage.h"
#include "ProcessLoader.h"
#include "ProcessInfo.h"
#include "ProcessDataAccess/ProcessInstructionReader.h"
#include "InfoMessage.h"
#include "Utils.h"
#include "Debugger.h"

#include <fstream>
#include <cereal/archives/json.hpp>


HomePage::HomePage()
{

}

HomePage::HomePage(RefPtr<Overlay>overlay) : overlay_(overlay)
{
	Ref<JSContext> context = overlay->view()->LockJSContext();
	SetJSContext(context.get());
	JSObject global = JSGlobalObject();

	loadDisAsmTable = global["loadDisAsmTable"];

	global["OnProcessLoad"] = BindJSCallback(&HomePage::OnProcessLoad);
	global["GetProcessBaseInfo"] = BindJSCallbackWithRetval(&HomePage::GetProcessBaseInfo);
	global["GetProcessInstructionByIndex"] = BindJSCallbackWithRetval(&HomePage::GetProcessInstructionByIndex);
	global["GetProcessInstructionByAddress"] = BindJSCallbackWithRetval(&HomePage::GetProcessInstructionByAddress);
	global["ToggleBreakPoint"] = BindJSCallbackWithRetval(&HomePage::ToggleBreakPoint);
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
	procInstReader->getInstructionByIndex((unsigned long long)procInfoInst->getProcessBaseAddress(), (unsigned long long)startIndex, count, instructions);

	/* Sprawdzic czy to na pewno to. */
	if (instructions.size() < count) {
		while (!instructions.empty()) {
			delete instructions.back();
			instructions.pop_back();
		}
		long long newStartIndex = startIndex - ( count - instructions.size());
		if (newStartIndex > 0) {
			procInstReader->getInstructionByIndex((unsigned long long)procInfoInst->getProcessBaseAddress(), (unsigned long long)newStartIndex, count, instructions);
		}
	}

	if (instructions.size() <= 0) {
		return JSValue(false);
	}

	int counter = 0;
	for(auto instuction = instructions.begin();instuction != instructions.end();instuction++) {
		(*instuction)->setInstructionIndex(startIndex + counter);
		counter++;
	}
	setBreakPointsForVec(instructions);
	std::vector<ASMInst> asmInstructions = GetInstToDisplay(&instructions, count);
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

	setBreakPointsForVec(instructions);
	std::vector<ASMInst> asmInstructions = GetInstToDisplay(&instructions, count);
	std::string strInstructions = Utils::serializeToJSON<std::vector<ASMInst>>(asmInstructions, "instructions");
	printf("Instructions: %s\n", strInstructions.c_str());

	return JSValue(strInstructions.c_str());
}

JSValue HomePage::ToggleBreakPoint(const JSObject& thisObject, const JSArgs& args) {
	if (args.size() != 1) {
		return JSValue(false);
	}
	int64_t startAddress = args[0].ToInteger();
	ProcessInfo* procInfoInst = &(ProcessInfo::getInstance());
	if (procInfoInst->getProcessBaseAddress() <= 0) {
		return JSValue(false);
	}

	std::vector<AssemblerInstruction*> instructions;
	ProcessInstructionReader* procInstReader = &(ProcessInstructionReader::getInstance());
	procInstReader->getInstructionByAddress((unsigned long long)startAddress, 1, instructions);
	if (instructions.size() != 1) {
		return JSValue(false);
	}

	Debugger* debuggerInstance = &(Debugger::getInstance());

	if (debuggerInstance->isBreakPointWithAddress(startAddress)) 
	{
		debuggerInstance->removeBeakPoint(instructions.at(0)->getOffset());
	} else {
		debuggerInstance->setBreakPoint(instructions.at(0)->getOffset());
	}

	return JSValue(true);
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



std::vector<ASMInst> HomePage::GetInstToDisplay(std::vector<AssemblerInstruction*>* instructions,int count) {
	std::vector<ASMInst> asmInstructions;
	int iterator = instructions->size() > count ? count : instructions->size();

	for (int i = 0; i < iterator; i++) {
		asmInstructions.push_back(instructions->at(i)->getStruct());
	}
	return asmInstructions;
}

