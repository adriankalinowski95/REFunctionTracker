#pragma once
#include <string>
#include <distorm.h>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

struct ASMInst 
{
	std::string mnemonic;
	std::string operands;
	std::string instructionHex;
	std::string offset;
	std::string instructionIndex;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("mnemonic", mnemonic)
				,cereal::make_nvp("operands", operands)
				,cereal::make_nvp("instructionHex", instructionHex)
				,cereal::make_nvp("offset", offset)
				,cereal::make_nvp("instructionIndex", instructionIndex));
	}
};

class AssemblerInstruction
{
public:
	AssemblerInstruction(int architecture,_DecodedInst* decodedInst);
	AssemblerInstruction();
	~AssemblerInstruction();

public:
	enum Architecture
	{
		X86Arch = 1,
		X64Arch = 2
	};

public:
	void lazyInit(int architecture, _DecodedInst* decodedInst);
	int getArchitecture();
	std::string getMnemonic();
	std::string getOperands();
	std::string getInstructionHex();
	std::string getOffsetString();
	unsigned int getSize();
	unsigned __int64 getOffset();
	_DecodedInst* getDecodedInst();
	int getOperandsLength();
	int getInstructionLength();
	ASMInst getStruct();
	std::string getAssemblerInstructionAsJSON();
	ASMInst jsonToASMInst(std::string json);
	void setInstructionIndex(int index);

private:
	_DecodedInst* decodedInst;
	int architecture; /*Architecture*/
	int instructionIndex;
};

