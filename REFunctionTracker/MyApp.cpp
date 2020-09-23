#include "MyApp.h"
#include <string>
#include <windows.h>
#include <Ultralight\CAPI.h>
#include <Ultralight\CAPI.h>

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 800
using namespace std;


std::string getExepath()
{
	TCHAR path[MAX_PATH + 1] = L"";
	DWORD len = GetCurrentDirectory(MAX_PATH, path);
	std::wstring ws(path);
	return std::string(ws.begin(), ws.end());
}

MyApp::MyApp() {
	///
	/// Create our main App instance.
	///
	app_ = App::Create();

	///
	/// Create a resizable window by passing by OR'ing our window flags with
	/// kWindowFlags_Resizable.
	///
	window_ = Window::Create(app_->main_monitor(), WINDOW_WIDTH, WINDOW_HEIGHT,
		false, kWindowFlags_Titled | kWindowFlags_Resizable);

	///
	/// Tell our app to use 'window' as our main window.
	///
	/// This call is required before creating any overlays or calling App::Run
	///
	app_->set_window(*window_.get());

	///
	/// Create our HTML overlay-- we don't care about its initial size and
	/// position because it'll be calculated when we call OnResize() below.
	///
	overlay_ = Overlay::Create(*window_.get(), 0, 0, 0, 0);

	///
	/// Force a call to OnResize to perform size/layout of our overlay.
	///
	OnResize(window_->width(), window_->height());

	///
	/// Load a page into our overlay's View
	///

	std::string path = std::string("file:///" + getExepath() + "/assets/" + "app.html");
	replace(path.begin(), path.end(), '\\', '/');
	ultralight::String ultralightStr = ultralight::String(path.c_str());
	overlay_->view()->LoadURL(ultralightStr);

	//overlay_->view()->LoadURL("file:///C:/Users/admin/source/repos/REFunctionTracker/REFunctionTracker/assets/app.html");
	//overlay_->view()->LoadURL("file:///app.html");
	///
	/// Register our MyApp instance as an AppListener so we can handle the
	/// App's OnUpdate event below.
	///
	app_->set_listener(this);

	///
	/// Register our MyApp instance as a WindowListener so we can handle the
	/// Window's OnResize event below.
	///
	window_->set_listener(this);

	///
	/// Register our MyApp instance as a LoadListener so we can handle the
	/// View's OnFinishLoading and OnDOMReady events below.
	///
	overlay_->view()->set_load_listener(this);

	///
	/// Register our MyApp instance as a ViewListener so we can handle the
	/// View's OnChangeCursor and OnChangeTitle events below.
	///
	overlay_->view()->set_view_listener(this);

	//RefPtr<Window> window2= Window::Create(app_->main_monitor(), WINDOW_WIDTH, WINDOW_HEIGHT,
	//    false, kWindowFlags_Titled | kWindowFlags_Resizable);
	//app_->window.
}






MyApp::~MyApp() {
}

void MyApp::Run() {
	app_->Run();
}

void MyApp::OnUpdate() {
	///
	/// This is called repeatedly from the application's update loop.
	///
	/// You should update any app logic here.
	///
}

void MyApp::OnClose() {
}

void MyApp::OnResize(uint32_t width, uint32_t height) {
	///
	/// This is called whenever the window changes size (values in pixels).
	///
	/// We resize our overlay here to take up the entire window.
	///
	overlay_->Resize(width, height);
}

void MyApp::OnFinishLoading(ultralight::View* caller,
	uint64_t frame_id,
	bool is_main_frame,
	const String& url) {
	///
	/// This is called when a frame finishes loading on the page.
	///
}

JSValue MyApp::GetMessageA(const JSObject& thisObject, const JSArgs& args) {
	///
	/// Return our message to JavaScript as a JSValue.
	///
	return JSValue("Hello from C++!<br/>Ultralight rocks!");
}

void MyApp::OnDOMReady(ultralight::View* caller,
	uint64_t frame_id,
	bool is_main_frame,
	const String& url) {
	Ref<JSContext> context = caller->LockJSContext();
	SetJSContext(context.get());

	///
	/// Get the global object (this would be the "window" object in JS)
	///
	JSObject global = JSGlobalObject();

	///
	/// Bind MyApp::GetMessage to the JavaScript function named "GetMessage".
	///
	/// You can get/set properties of JSObjects by using the [] operator with
	/// the following types as potential property values:
	///  - JSValue
	///      Represents a JavaScript value, eg String, Object, Function, etc.
	///  - JSCallback 
	///      Typedef of std::function<void(const JSObject&, const JSArgs&)>)
	///  - JSCallbackWithRetval 
	///      Typedef of std::function<JSValue(const JSObject&, const JSArgs&)>)
	///
	/// We use the BindJSCallbackWithRetval macro to bind our C++ class member
	/// function to our JavaScript callback.
	///
	global["GetMessage"] = BindJSCallbackWithRetval(&MyApp::GetMessageA);
}

void MyApp::OnChangeCursor(ultralight::View* caller,
	Cursor cursor) {
	///
	/// This is called whenever the page requests to change the cursor.
	///
	/// We update the main window's cursor here.
	///
	window_->SetCursor(cursor);
}

void MyApp::OnChangeTitle(ultralight::View* caller,
	const String& title) {
	///
	/// This is called whenever the page requests to change the title.
	///
	/// We update the main window's title here.
	///
	window_->SetTitle(title.utf8().data());
}
