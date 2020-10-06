#include "Disassembler.h"

#define MAX_INSTRUCTIONS (2000)
#define BUFFER_SIZE 8000
#define PROCESS_SIZE_BUFFER 16000

Disassembler::Disassembler()
{

}

Disassembler::~Disassembler()
{
}

int Disassembler::dissassembly(DWORD_PTR codeOffset, const unsigned char* code, int codeLen, int architecture,std::vector<AssemblerInstruction*>&instructions)
{
	_DecodeType dt;
	_DecodeResult res;
	_DecodedInst decodedInstructions[MAX_INSTRUCTIONS];
	unsigned int usedInstructionsCount;

	if (architecture == Disassembler::X64Arch)
	{
		 dt = Decode64Bits;
	}
	else if(architecture == Disassembler::X86Arch)
	{
		 dt = Decode32Bits;
	}
	else
	{
		return  DISSASSEMBLER_ERROR;
	}
	
	res = distorm_decode64(codeOffset, code, codeLen, dt, decodedInstructions, MAX_INSTRUCTIONS,&usedInstructionsCount);

	if (res == DECRES_INPUTERR)
	{
		return DISSASSEMBLER_ERROR;
	}


	for (int i = 0; i <usedInstructionsCount; i++)
	{
		_DecodedInst* newDecodedInst = new _DecodedInst;
		memcpy(newDecodedInst,&decodedInstructions[i], sizeof(_DecodedInst));
		AssemblerInstruction* newAsmInst = new AssemblerInstruction(architecture,newDecodedInst);
		instructions.push_back(newAsmInst);
	}

	return DISSASSEMBLER_SUCESS;
}



