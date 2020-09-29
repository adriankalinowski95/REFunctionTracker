#include "SelectProcessDialog.h"
#include "ProcessLoader.h"
#include <fstream>
#include <cereal/archives/json.hpp>
#include <vector>
#include "Utils.h"

SelectProcessDialog::SelectProcessDialog()
{

}

SelectProcessDialog::SelectProcessDialog(RefPtr<Overlay>overlay) : overlay_(overlay)
{
	Ref<JSContext> context = overlay->view()->LockJSContext();
	SetJSContext(context.get());
	JSObject global = JSGlobalObject();
	selectProcessTest = global["selectProcessTest"];
	global["GetProcessList"] = BindJSCallbackWithRetval(&SelectProcessDialog::GetProcessList);
}


SelectProcessDialog::~SelectProcessDialog()
{

}

JSValue SelectProcessDialog::GetProcessList(const JSObject& thisObject, const JSArgs& args) 
{
	ProcessLoader* procLoadInst = &(ProcessLoader::getInstance());

	//selectProcessTest({ procLoadInst. });
	return JSValue(procLoadInst->getProcessListAsJSON().c_str());
}