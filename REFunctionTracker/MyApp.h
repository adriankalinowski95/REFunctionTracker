#pragma once
#include <AppCore/AppCore.h>
#include "SelectProcessDialog.h"
#include "HomePage.h"
using namespace ultralight;

class MyApp : public AppListener,
              public WindowListener,
              public LoadListener,
              public ViewListener {
public:
  MyApp();
  void InitModules();
  void ToggleInspector();
  void Resize(uint32_t width, uint32_t height);
  Ref<View> view() { return overlay_->view(); }
  virtual ~MyApp();

  // Start the run loop.
  virtual void Run();

  // This is called continuously from the app's main loop.
  virtual void OnUpdate() override;

  // This is called when the window is closing.
  virtual void OnClose() override;

  // This is called whenever the window resizes.
  virtual void OnResize(uint32_t width, uint32_t height) override;

  // This is called when the page finishes a load in one of its frames.
  virtual void OnFinishLoading(ultralight::View* caller,
                               uint64_t frame_id,
                               bool is_main_frame,
                               const String& url) override;

  // This is called when the DOM has loaded in one of its frames.
  virtual void OnDOMReady(ultralight::View* caller,
                          uint64_t frame_id,
                          bool is_main_frame,
                          const String& url) override;

  // This is called when the page requests to change the Cursor.
  virtual void OnChangeCursor(ultralight::View* caller,
    Cursor cursor) override;

  virtual void OnChangeTitle(ultralight::View* caller,
    const String& title) override;

  void OnToggleTools(const JSObject& obj, const JSArgs& args);
  JSValue GetMessageA(const JSObject& thisObject, const JSArgs& args);
  
protected:
  RefPtr<App> app_;
  RefPtr<Window> window_;
  RefPtr<Overlay> overlay_;
  RefPtr<View> view_;
  RefPtr<Overlay> inspector_overlay_;
  uint32_t container_width_, container_height_;
  Settings settings;
  JSFunction testFunction;
  SelectProcessDialog* selectProcessDialog_;
  HomePage* homePage_;
};
