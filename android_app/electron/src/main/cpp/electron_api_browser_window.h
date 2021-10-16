#ifndef ANDROID_APP_ELECTRON_API_BROWSER_WINDOW_H
#define ANDROID_APP_ELECTRON_API_BROWSER_WINDOW_H

#include "handle.h"
#include "trackable_object.h"

class BrowserWindow : public gin_helper::TrackableObject<BrowserWindow> {

public:
    BrowserWindow(const v8::FunctionCallbackInfo<v8::Value> &info);

    gin_helper::ObjectTemplateBuilder GetObjectTemplateBuilder(v8::Isolate *isolate) override;

    const char *GetTypeName() override;

    static void BuildPrototype(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> prototype);

private:
    void LoadURL(const std::string &url);

    int web_contents_id_;
};

template<>
struct gin::Converter<BrowserWindow*> {
    static bool FromV8(v8::Isolate *isolate,
                       v8::Local<v8::Value> val,
                       BrowserWindow **out);
};

#endif //ANDROID_APP_ELECTRON_API_BROWSER_WINDOW_H
