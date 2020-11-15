#pragma once
#include <AppCore/JSHelpers.h>
#include <AppCore/AppCore.h>

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
	JSValue GetProcessInstructionsCount(const JSObject& thisObject, const JSArgs& args);
	JSValue GetProcessInstructionByIndex(const JSObject& thisObject, const JSArgs& args);
	//JSValue GetProcessList(const JSObject& thisObject, const JSArgs& args);
private:
	RefPtr<Overlay>overlay_;
	JSFunction loadDisAsmTable;
};

