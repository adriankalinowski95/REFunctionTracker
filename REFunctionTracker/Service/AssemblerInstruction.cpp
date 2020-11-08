#include "AssemblerInstruction.h"
#include "Utils.h"

#define INSTURCTION_ADDRESS_SIZE 50

AssemblerInstruction::AssemblerInstruction(int architecture, _DecodedInst* decodedInst):architecture(architecture),decodedInst(decodedInst)
{

}

AssemblerInstruction::AssemblerInstruction()
{
	decodedInst = NULL;
	architecture = 0;
}

AssemblerInstruction::~AssemblerInstruction()
{
	if (decodedInst != NULL)
	{
		delete decodedInst;
	}
}

void AssemblerInstruction::lazyInit(int architecture, _DecodedInst* decodedInst)
{
	this->architecture = architecture;
	this->decodedInst = decodedInst;
}

int AssemblerInstruction::getArchitecture()
{
	return this->architecture;
}

std::string AssemblerInstruction::getMnemonic()
{
	return std::string((const char*)decodedInst->mnemonic.p);
}

std::string AssemblerInstruction::getOperands()
{
	return std::string((const char*)decodedInst->operands.p);
}

std::string AssemblerInstruction::getInstructionHex()
{
	return std::string((const char*)decodedInst->instructionHex.p);
}

std::string AssemblerInstruction::getOffsetString()
{
	char instructionAddress[INSTURCTION_ADDRESS_SIZE];
	sprintf_s(instructionAddress, "0x%I64X", this->decodedInst->offset);
	return std::string(instructionAddress);
}

unsigned int AssemblerInstruction::getSize()
{
	return decodedInst->size;
}

unsigned __int64 AssemblerInstruction::getOffset()
{
	return decodedInst->offset;
}

_DecodedInst* AssemblerInstruction::getDecodedInst()
{
	return this->decodedInst;
}

int AssemblerInstruction::getOperandsLength()
{
	return this->decodedInst->operands.length;
}

int AssemblerInstruction::getInstructionLength()
{
	return this->decodedInst->mnemonic.length;
}

ASMInst AssemblerInstruction::getStruct()
{
	ASMInst asmInst;
	asmInst.mnemonic = this->getMnemonic();
	asmInst.operands = this->getOperands();
	asmInst.instructionHex = this->getInstructionHex();
	asmInst.offset = this->getOffsetString();
	return asmInst;
}

std::string AssemblerInstruction::getAssemblerInstructionAsJSON()
{
	return Utils::serializeToJSON<ASMInst>(this->getStruct(), "assemblerInstruction").c_str();
}

ASMInst AssemblerInstruction::jsonToASMInst(std::string json)
{
	return Utils::serializeToObject<ASMInst>(json);
}
