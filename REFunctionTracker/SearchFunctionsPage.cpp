#include "SearchFunctionsPage.h"
#include <ProcessDataAccess/ProcessInstructionReader.h>
#include "Utils.h"

SearchFunctionsPage::SearchFunctionsPage() {}

SearchFunctionsPage::SearchFunctionsPage(RefPtr<Overlay> overlay): overlay_(overlay) {
	Ref<JSContext> context = overlay->view()->LockJSContext();
	SetJSContext(context.get());
	JSObject global = JSGlobalObject();

	//loadDisAsmTable = global["loadDisAsmTable"];
	global["LoadCallFunctions"] = BindJSCallback(&SearchFunctionsPage::LoadCallFunctions);
	global["GetSearchFunctionMax"] = BindJSCallback(&SearchFunctionsPage::GetSearchFunctionMax);
}

SearchFunctionsPage::~SearchFunctionsPage() {}

void SearchFunctionsPage::OnProcessLoad(const JSObject& obj, const JSArgs& args) {

}

JSValue SearchFunctionsPage::LoadCallFunctions(const JSObject& obj, const JSArgs& args) {
	if(args.size() != 2) {
		return JSValue(false);
	}
	unsigned long long startIndex = args[0];
	int count = args[1];
	if(startIndex < 0 || count <= 0) {
		return JSValue(false);
	}
	ProcessInfo* procInfoInst = &( ProcessInfo::getInstance() );
	unsigned long long processBaseAddress = (unsigned long long)procInfoInst->getProcessBaseAddress();
	if(processBaseAddress <= 0) {
		return JSValue(false);
	}
	std::vector<AssemblerInstruction*> instructions;
	ProcessInstructionReader* procInstReader = &( ProcessInstructionReader::getInstance() );
	procInstReader->getByOperand(processBaseAddress, startIndex,count, instructions,"CALL");

	std::vector<ASMInst> asmInstructions = GetInstToDisplay(&instructions, count);
	std::string strInstructions = Utils::serializeToJSON<std::vector<ASMInst>>(asmInstructions, "instructions");
	printf("Instructions: %s\n", strInstructions.c_str());

}

std::vector<ASMInst> SearchFunctionsPage::GetInstToDisplay(std::vector<AssemblerInstruction*>* instructions, int count) {
	std::vector<ASMInst> asmInstructions;
	int iterator = instructions->size() > count ? count : instructions->size();

	for(int i = 0; i < iterator; i++) {
		asmInstructions.push_back(instructions->at(i)->getStruct());
	}
	return asmInstructions;
}

JSValue SearchFunctionsPage::GetSearchFunctionMax(const JSObject& thisObject, const JSArgs& args) {
	ProcessInfo* procInfoInst = &( ProcessInfo::getInstance() );
	unsigned long long processBaseAddress = ( unsigned long long )procInfoInst->getProcessBaseAddress();
	if(processBaseAddress <= 0) {
		return JSValue(false);
	}
	std::vector<AssemblerInstruction*> instructions;
	ProcessInstructionReader* procInstReader = &( ProcessInstructionReader::getInstance() );
	unsigned long long maxOperands = procInstReader->getMaxByOperand(processBaseAddress, "CALL");
	return JSValue(maxOperands);
}


