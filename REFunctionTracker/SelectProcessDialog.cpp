#include "SelectProcessDialog.h"

SelectProcessDialog::SelectProcessDialog()
{

}

SelectProcessDialog::SelectProcessDialog(RefPtr<Overlay>overlay) : overlay_(overlay)
{
	Ref<JSContext> context = overlay->view()->LockJSContext();
	SetJSContext(context.get());
	JSObject global = JSGlobalObject();
	selectProcessTest = global["selectProcessTest"];
	global["GetMessageFromSelectProcess"] = BindJSCallbackWithRetval(&SelectProcessDialog::GetMessageFromSelectProcess);
}


SelectProcessDialog::~SelectProcessDialog()
{

}

JSValue SelectProcessDialog::GetMessageFromSelectProcess(const JSObject& thisObject, const JSArgs& args) 
{
	printf("Get message from select process");
	selectProcessTest({ 123 });
	return JSValue("Hello from C++!<br/>Ultralight rocks!");
}