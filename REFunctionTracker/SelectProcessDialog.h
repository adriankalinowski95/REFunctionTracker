#pragma once
#include <AppCore/JSHelpers.h>
#include <AppCore/AppCore.h>

using ultralight::JSObject;
using ultralight::JSArgs;
using ultralight::JSFunction;
using namespace ultralight;

class SelectProcessDialog
{
public:
	SelectProcessDialog();
	SelectProcessDialog(RefPtr<Overlay>overlay);
	~SelectProcessDialog();

	JSValue GetProcessList(const JSObject& thisObject, const JSArgs& args);
	JSValue SetProcess(const JSObject& thisObject, const JSArgs& args);

private:
	RefPtr<Overlay>overlay_;
	JSFunction selectProcessTest;
};

