#pragma once
#include <AppCore/JSHelpers.h>
#include <AppCore/AppCore.h>
#include <vector>
#include "AssemblerInstruction.h"

using ultralight::JSObject;
using ultralight::JSArgs;
using ultralight::JSFunction;
using namespace ultralight;

class SearchFunctionsPage {
public:
	SearchFunctionsPage();
	SearchFunctionsPage(RefPtr<Overlay>overlay);
	~SearchFunctionsPage();

	void OnProcessLoad(const JSObject& obj, const JSArgs& args);
	JSValue LoadCallFunctions(const JSObject& obj, const JSArgs& args);
	JSValue GetSearchFunctionMax(const JSObject& thisObject, const JSArgs& args);
private:
	RefPtr<Overlay>overlay_;
	std::vector<ASMInst> GetInstToDisplay(std::vector<AssemblerInstruction*>* instructions, int count);
};

