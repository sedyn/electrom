#include "electron_api_app.h"

using namespace v8;

App *app() {
    static auto *electronApp = new App;
    return electronApp;
}

const char *App::GetTypeName() {
    return "App";
}

v8::Local<v8::ObjectTemplate> App::GetObjectTemplate(v8::Isolate *isolate) {
    Local<ObjectTemplate> tmpl = EventEmitterMixin<App>::GetObjectTemplate(isolate);
    return tmpl;
}

namespace {

    void Initialize(v8::Local<v8::Object> exports,
                    v8::Local<v8::Value> unused,
                    v8::Local<v8::Context> context) {
        Isolate *isolate = context->GetIsolate();
        Local<ObjectTemplate> tmpl = app()->GetObjectTemplate(isolate);

        exports->Set(
            String::NewFromUtf8(isolate, "app"),
            (Local<Value>) tmpl->NewInstance(context).ToLocalChecked()
        );
    }

}

NODE_LINKED_MODULE_CONTEXT_AWARE(electron_browser_app, Initialize)
