#pragma once
#include <AppCore/JSHelpers.h>
#include <AppCore/AppCore.h>
#include <vector>
#include "AssemblerInstruction.h"

using ultralight::JSObject;
using ultralight::JSArgs;
using ultralight::JSFunction;
using namespace ultralight;


class HomePage
{
public:
	HomePage();
	HomePage(RefPtr<Overlay>overlay);
	~HomePage();

	void OnProcessLoad(const JSObject& obj, const JSArgs& args);
	JSValue GetProcessBaseInfo(const JSObject& thisObject, const JSArgs& args);
	JSValue GetProcessInstructionByIndex(const JSObject& thisObject, const JSArgs& args);
	JSValue GetProcessInstructionByAddress(const JSObject& thisObject, const JSArgs& args);
	JSValue ToggleBreakPoint(const JSObject& thisObject, const JSArgs& args);
	std::vector<ASMInst> GetInstToDisplay(std::vector<AssemblerInstruction*>* instructions, int count);
	void UpdateInstructionsByBreakPoints(std::vector<AssemblerInstruction*>* instructions);
	//JSValue GetProcessList(const JSObject& thisObject, const JSArgs& args);
private:
	RefPtr<Overlay>overlay_;
	JSFunction loadDisAsmTable;
	int getBreakPointsInRange(int startIndex, int count);
};

