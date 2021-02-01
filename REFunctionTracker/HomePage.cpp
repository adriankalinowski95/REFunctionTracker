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

/* @Todo Tutaj skonczy³em*/
bool isBreakPoint(AssemblerInstruction* instruction) {
	Debugger* debuggerInstance = &( Debugger::getInstance() );
	std::vector<BreakPoint_Typedef>& breakPoints = debuggerInstance->getCurrentBreakPoins();
	for(auto breakPoint = breakPoints.begin(); breakPoint != breakPoints.end();breakPoint++) {
		if(breakPoint->address == instruction->getOffset()) {
			return true;
		}
	}
	return false;
}
bool isBreakPointByAddress(unsigned long long address) {
	Debugger* debuggerInstance = &( Debugger::getInstance() );
	std::vector<BreakPoint_Typedef>& breakPoints = debuggerInstance->getCurrentBreakPoins();
	for(auto breakPoint = breakPoints.begin(); breakPoint != breakPoints.end();breakPoint++) {
		if(breakPoint->address == address) {
			return true;
		}
	}
	return false;
}
bool isBreakPointInVec(std::vector<AssemblerInstruction*>& instructions) {
	Debugger* debuggerInstance = &( Debugger::getInstance() );
	std::vector<BreakPoint_Typedef>& breakPoints = debuggerInstance->getCurrentBreakPoins();
	for(auto instruction = instructions.begin(); instruction != instructions.end();instruction++) {
		if(isBreakPoint(*instruction)) {
			return true;
		}
	}
	return false;
}
AssemblerInstruction getInstBPByOffset(unsigned long long address) {
	Debugger* debuggerInstance = &( Debugger::getInstance() );
	std::vector<BreakPoint_Typedef>& breakPoints = debuggerInstance->getCurrentBreakPoins();
	for(auto breakPoint = breakPoints.begin(); breakPoint != breakPoints.end();breakPoint++) {
		if(breakPoint->address == address) {
			return AssemblerInstruction(*breakPoint->prevInst);
		}
	}
	return AssemblerInstruction();
}
bool isBreakPointInstBefore(unsigned long long address) {
	Debugger* debuggerInstance = &( Debugger::getInstance() );
	std::vector<BreakPoint_Typedef>& breakPoints = debuggerInstance->getCurrentBreakPoins();
	for(auto breakPoint = breakPoints.begin(); breakPoint != breakPoints.end();breakPoint++) {
		if(breakPoint->address < address && (breakPoint->address + breakPoint->prevInst->getSize()) > address ) {
			return true;
		}
	}
	return false;
}
unsigned long long getBreakPointsBeforeInstAddr(unsigned long long address) {
	Debugger* debuggerInstance = &( Debugger::getInstance() );
	std::vector<BreakPoint_Typedef>& breakPoints = debuggerInstance->getCurrentBreakPoins();
	for(auto breakPoint = breakPoints.begin(); breakPoint != breakPoints.end();breakPoint++) {
		if(breakPoint->address < address && ( breakPoint->address + breakPoint->prevInst->getSize() ) > address) {
			/* Tutaj trzeba uwzglednic fakt ¿e instrukcja przed te¿ moze miescic sie w zakresie break pointa */
			return breakPoint->address;
		}
	}
	return -1;
}

/*
unsigned long long getBreakPointsBeforeInstAddr2(unsigned long long address) {
	unsigned long long newAddress;
	ProcessInfo* procInfoInst = &( ProcessInfo::getInstance() );
	ProcessInstructionReader* procInstReader = &( ProcessInstructionReader::getInstance());
	long index = 0;
	while(index >= 0) {
		newAddress = getBreakPointsBeforeInstAddr(address);
		//index = procInstReader->getInstructionIndex(( unsigned long long )procInfoInst->getProcessBaseAddress(), newAddress);
		//index --;
		AssemblerInstruction asmInst = getInstBPByOffset(newAddress);
		//procInstReader->getOneInstByIndex(index, asmInst);
		return asmInst.getOffset() + asmInst.getSize();
	}

}*/
AssemblerInstruction getBreakPointsBeforeInstAddr2(unsigned long long address) {
	unsigned long long newAddress;
	ProcessInfo* procInfoInst = &( ProcessInfo::getInstance() );
	ProcessInstructionReader* procInstReader = &( ProcessInstructionReader::getInstance() );
	long index = 0;
	while(index >= 0) {
		newAddress = getBreakPointsBeforeInstAddr(address);
		//index = procInstReader->getInstructionIndex(( unsigned long long )procInfoInst->getProcessBaseAddress(), newAddress);
		//index --;
		AssemblerInstruction asmInst = getInstBPByOffset(newAddress);
		index = procInstReader->getInstructionIndex(( unsigned long long )procInfoInst->getProcessBaseAddress(), asmInst.getOffset());
		asmInst.setInstructionIndex(index);
		//procInstReader->getOneInstByIndex(index, asmInst);
		return asmInst;
	}

}

/* Tutaj zmienic na podobna jak w read by address*/
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
	/* Kolejna rozkmina, przed diassemblingiem je¿eli w tkaim zakresie znajduje siê bp to trzeba go podmienic :O  */
	//if(!isBreakPointInstBefore(instructions[0]->getOffset()) && !isBreakPointInVec(instructions)){
	{
		int count = 0;
		for(auto instuction = instructions.begin();instuction != instructions.end();instuction++) {
			(*instuction)->setInstructionIndex(startIndex + count);
			count ++;
			if(isBreakPoint((*instuction))) {
				(*instuction )->setIsBreakPoint(true);
			}
		}
		std::vector<ASMInst> asmInstructions = GetInstToDisplay(&instructions, count);
		std::string strInstructions = Utils::serializeToJSON<std::vector<ASMInst>>(asmInstructions, "instructions");
		printf("Instructions: %s\n", strInstructions.c_str());

		return JSValue(strInstructions.c_str());
	}
	//}
	
	//unsigned long long currentOffset = instructions[0]->getOffset();
	//long index;
	///* Obliczanie na podstawie najblizszego, ale co zrobiæ, gdy jest taka sama odlegloœæ? */
	//
	//if(isBreakPointInstBefore(instructions[0]->getOffset())) {
	//	//currentOffset = getBreakPointsBeforeInstAddr2(instructions[0]->getOffset());
	//	AssemblerInstruction asmInst = getBreakPointsBeforeInstAddr2(instructions[0]->getOffset());
	//	index = procInstReader->getInstructionIndex(( unsigned long long )procInfoInst->getProcessBaseAddress(), asmInst.getOffset());
	//	
	//	/* Prawie dziala, ale nie zawsze da sie okreslic indexy dwoch instrukcji */
	//	if(( startIndex - index  ) != 1) {
	//		currentOffset = asmInst.getOffset();
	//		index = procInstReader->getInstructionIndex(( unsigned long long )procInfoInst->getProcessBaseAddress(), currentOffset);
	//	}
	//	else {
	//		currentOffset = asmInst.getOffset() + asmInst.getSize();
	//		index = procInstReader->getInstructionIndex(( unsigned long long )procInfoInst->getProcessBaseAddress(), currentOffset);
	//	}
	//}
	//else {
	//	index = startIndex;
	//}
	////index = procInstReader->getInstructionIndex(( unsigned long long )procInfoInst->getProcessBaseAddress(), currentOffset);
	//instructions.erase(instructions.begin(), instructions.end());
	//while(instructions.size() < count) {
	//	if(isBreakPointByAddress(currentOffset)) {
	//		AssemblerInstruction instruction = getInstBPByOffset(currentOffset);
	//		instruction.setIsBreakPoint(true);
	//		instruction.setInstructionIndex(index);
	//		instructions.push_back(new AssemblerInstruction(instruction));
	//		currentOffset += instruction.getSize();
	//		index++;
	//		continue;
	//	}
	//	AssemblerInstruction instruction;
	//	if(!procInstReader->getOneInstByAddress(currentOffset, instruction)) {
	//		break;
	//	}
	//	instruction.setInstructionIndex(index);
	//	instructions.push_back(new AssemblerInstruction(instruction));
	//	currentOffset += instruction.getSize();
	//	index++;
	//}

	//std::vector<ASMInst> asmInstructions = GetInstToDisplay(&instructions, count);
	//std::string strInstructions = Utils::serializeToJSON<std::vector<ASMInst>>(asmInstructions, "instructions");
	//printf("Instructions: %s\n", strInstructions.c_str());

	//return JSValue(strInstructions.c_str());
	
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

	/* Remove asm instructions */
	std::vector<AssemblerInstruction*> instructions;
	ProcessInstructionReader* procInstReader = &(ProcessInstructionReader::getInstance());

	
	procInstReader->getInstructionByAddress(startAddress, count, instructions);
	if (instructions.size() <= 0) {
		return JSValue(false);
	}

	/* Tutaj sprawdzic czy w instrukcjach znajduje siê break pointy, je¿eli nie to zwróc instrukcje*/
	//if(!isBreakPointInVec(instructions)) {
		for(auto instuction = instructions.begin();instuction != instructions.end();instuction++) {
			if(isBreakPoint(( *instuction ))){
				( *instuction )->setIsBreakPoint(true);
			}
		}
		std::vector<ASMInst> asmInstructions = GetInstToDisplay(&instructions, count);
		std::string strInstructions = Utils::serializeToJSON<std::vector<ASMInst>>(asmInstructions, "instructions");
		printf("Instructions: %s\n", strInstructions.c_str());

		return JSValue(strInstructions.c_str());
	//}
	/*
	instructions.erase(instructions.begin(),instructions.end());
	unsigned long long currentOffset = startAddress;
	long index = procInstReader->getInstructionIndex(( unsigned long long )procInfoInst->getProcessBaseAddress(), currentOffset);
	while(instructions.size() < count) {
		if(isBreakPointByAddress(currentOffset)) {
			AssemblerInstruction instruction = getInstBPByOffset(currentOffset);
			instruction.setIsBreakPoint(true);
			instruction.setInstructionIndex(index);
			instructions.push_back(new AssemblerInstruction(instruction));
			currentOffset += instruction.getSize();
			index++;
			continue;
		}
		AssemblerInstruction instruction;
		if(!procInstReader->getOneInstByAddress(currentOffset, instruction)){
			break;
		}
		instruction.setInstructionIndex(index);
		instructions.push_back(new AssemblerInstruction(instruction));
		currentOffset += instruction.getSize();
		index++;
	}

	std::vector<ASMInst> asmInstructions = GetInstToDisplay(&instructions, count);
	std::string strInstructions = Utils::serializeToJSON<std::vector<ASMInst>>(asmInstructions, "instructions");
	printf("Instructions: %s\n", strInstructions.c_str());

	return JSValue(strInstructions.c_str());*/
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
	//startAddress = startAddress + procInfoInst->getProcessBaseAddress();
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

int HomePage::getBreakPointsInRange(int startIndex,int count) {
	int aviableBreakPointsInRange = 0;
	Debugger* debuggerInstance = &(Debugger::getInstance());
	std::vector<BreakPoint_Typedef> breakPoints = debuggerInstance->getCurrentBreakPoins();
	for (int i = 0; i < breakPoints.size(); i++) {
		if (breakPoints.at(i).index >= startIndex && breakPoints.at(i).index < (startIndex + count))
		{
			aviableBreakPointsInRange++;
		}
	}
	return aviableBreakPointsInRange;
}

/* A mo¿e pousuwaæ breakpointy, a nastêpnie je wpisac ponownie? 
 * Algorytm:
 * Zatrzymaj proces
 * pobierz odpowiednia ilosc.
 * Sprawdz czy sa tam break pointy
 * Jak s¹ to zrob liste breakpointów
 * 
*/
void HomePage::UpdateInstructionsByBreakPoints(std::vector<AssemblerInstruction*>* instructions) {
	Debugger* debuggerInstance = &(Debugger::getInstance());
	std::vector<BreakPoint_Typedef> breakPoints = debuggerInstance->getCurrentBreakPoins();
	std::vector< AssemblerInstruction*> instAsBeakPoints;
	for (int i = 0; i < instructions->size(); i++) {
		for (int j = 0; j < breakPoints.size(); j++) {
			if (breakPoints.at(j).address == instructions->at(i)->getOffset()) {
				instructions->at(i)->setDecodeInst(breakPoints.at(j).prevInst->getDecodedInst());
				instructions->at(i)->setIsBreakPoint(true);
				instAsBeakPoints.push_back(instructions->at(i));
			}
		}
	}

	int iterator = 0;
	unsigned long long intBreakPointRangeStart = 0;
	unsigned long long intBreakPointRangeStop = 0;
	for (int i = 0; i < instAsBeakPoints.size(); i++) {
		intBreakPointRangeStart = instAsBeakPoints.at(i)->getOffset();
		intBreakPointRangeStop = intBreakPointRangeStart + instAsBeakPoints.at(i)->getDecodedInst()->size;
		iterator = 0;
		while (iterator < instructions->size()) {
			if (instructions->at(iterator)->getOffset() == instAsBeakPoints.at(i)->getOffset()) {
				iterator++;
				continue;
			}
			if (instructions->at(iterator)->getOffset() > intBreakPointRangeStart &&
				instructions->at(iterator)->getOffset() < intBreakPointRangeStop) {
				instructions->erase(instructions->begin() + iterator);
				iterator = 0;
				continue;
			}
			iterator++;
		}
	}

	/* Add _Decode free. */
}

std::vector<ASMInst> HomePage::GetInstToDisplay(std::vector<AssemblerInstruction*>* instructions,int count) {
	std::vector<ASMInst> asmInstructions;
	int iterator = instructions->size() > count ? count : instructions->size();

	for (int i = 0; i < iterator; i++) {
		asmInstructions.push_back(instructions->at(i)->getStruct());
	}
	return asmInstructions;
}

