#include "HomePage.h"
#include "ProcessLoader.h"
#include "ProcessInfo.h"
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
}


HomePage::~HomePage()
{

}

void HomePage::OnProcessLoad(const JSObject& obj, const JSArgs& args)
{
	ProcessLoader* procLoadInst = &(ProcessLoader::getInstance());
	ProcessInfo* procInfoInst = &(ProcessInfo::getInstance());
	procInfoInst->setProcessHandle(procLoadInst->getProcessHandle());
	

}