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
	global["SetProcess"] = BindJSCallbackWithRetval(&SelectProcessDialog::SetProcess);
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

JSValue SelectProcessDialog::SetProcess(const JSObject& thisObject, const JSArgs& args)
{
	if (args.size() == 1) {
		//uint64_t processPID = args[0];
		//printf("process : %d \n", args[0]);
		ProcessLoader* procLoadInst = &(ProcessLoader::getInstance());

		ultralight::String stringProcess = args[0];
		std::string processDataJson = stringProcess.utf8().data();
		ProcData processData = procLoadInst->jsonToProcData(processDataJson);
		if (processData.processPID == 0) {
			return JSValue("Error");
		}
		
		HANDLE procHandle = procLoadInst->loadProcessByPID(processData.processPID);
		if (procHandle != NULL) {
			return JSValue("Success");
		}
		
	}
	return JSValue("Error");
}