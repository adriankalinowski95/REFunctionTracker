#include "SelectProcessDialog.h"

SelectProcessDialog::SelectProcessDialog()
{

}

SelectProcessDialog::SelectProcessDialog(RefPtr<Overlay>overlay) : overlay_(overlay)
{
	//this->overlay_->view()->set_load_listener(this);
	//this->overlay_->view()->set_load_listener(this);
}


SelectProcessDialog::~SelectProcessDialog()
{
}

void SelectProcessDialog::OnDOMReady(View* caller, uint64_t frame_id, bool is_main_frame, const String& url)
{
	Ref<JSContext> context = caller->LockJSContext();
	SetJSContext(context.get());
	JSObject global = JSGlobalObject();
	selectProcessTest = global["selectProcessTest"];
	global["GetMessageFromSelectProcess"] = BindJSCallbackWithRetval(&SelectProcessDialog::GetMessageFromSelectProcess);
}

JSValue SelectProcessDialog::GetMessageFromSelectProcess(const JSObject& thisObject, const JSArgs& args) {
	printf("Get message from select process");
	selectProcessTest({ 123 });
	return JSValue("Hello from C++!<br/>Ultralight rocks!");
}