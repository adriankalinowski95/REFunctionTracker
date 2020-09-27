#pragma once
#include <AppCore/JSHelpers.h>
#include <AppCore/AppCore.h>

using ultralight::JSObject;
using ultralight::JSArgs;
using ultralight::JSFunction;
using namespace ultralight;

class SelectProcessDialog : public LoadListener
{
public:
	SelectProcessDialog();
	SelectProcessDialog(RefPtr<Overlay>overlay);
	~SelectProcessDialog();

	virtual void OnDOMReady(View* caller, uint64_t frame_id,
		bool is_main_frame, const String& url) override;
	JSValue GetMessageFromSelectProcess(const JSObject& thisObject, const JSArgs& args);

private:
	RefPtr<Overlay>overlay_;
	JSFunction selectProcessTest;
};

