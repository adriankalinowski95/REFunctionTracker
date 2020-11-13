#include "HomePage.h"
#include "ProcessLoader.h"
#include "ProcessInfo.h"
#include "ProcessDataAccess/ProcessInstructionReader.h"
#include "InfoMessage.h"
#include "Utils.h"

#include <fstream>
#include <cereal/archives/json.hpp>
#include <vector>


HomePage::HomePage()
{

}

HomePage::HomePage(RefPtr<Overlay>overlay) : overlay_(overlay)
{
	Ref<JSContext> context = overlay->view()->LockJSContext();
	SetJSContext(context.get());
	JSObject global = JSGlobalObject();
	global["OnProcessLoad"] = BindJSCallback(&HomePage::OnProcessLoad);
	loadDisAsmTable = global["loadDisAsmTable"];

	global["GetProcessInstructionsCount"] = BindJSCallbackWithRetval(&HomePage::GetProcessInstructionsCount);
}

JSValue HomePage::GetProcessInstructionsCount(const JSObject& thisObject, const JSArgs& args)
{
	ProcessLoader* procLoadInst = &(ProcessLoader::getInstance());
	ProcessInfo* procInfoInst = &(ProcessInfo::getInstance());
	procInfoInst->loadProcessBaseInformation64();

	ProcessInstructionReader* procInstReader = &(ProcessInstructionReader::getInstance());
	unsigned long long instCount = procInstReader->getProcessInstructionCount((unsigned long long)procInfoInst->getProcessBaseAddress());
	printf("Inst count %d \n", instCount);

	return JSValue(instCount);
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


