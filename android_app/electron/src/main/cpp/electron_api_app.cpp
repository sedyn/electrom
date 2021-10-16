#include "electron_api_app.h"

using namespace v8;

App *App::Get() {
    static auto *app = new App;
    return app;
}

gin::Handle<App> App::Create(v8::Isolate *isolate) {
    return gin::CreateHandle(isolate, Get());
}

const char *App::GetTypeName() {
    return "App";
}

gin_helper::ObjectTemplateBuilder App::GetObjectTemplateBuilder(v8::Isolate *isolate) {
    return EventEmitterMixin<App>::GetObjectTemplateBuilder(isolate);
}

bool App::EmitReady() {
    return Emit("ready");
}

namespace {

    void Initialize(v8::Local<v8::Object> exports,
                    v8::Local<v8::Value> unused,
                    v8::Local<v8::Context> context) {
        Isolate *isolate = context->GetIsolate();

        exports->Set(helper::StringToSymbol(isolate, "app"), App::Create(isolate).ToV8());
    }

}

NODE_LINKED_MODULE_CONTEXT_AWARE(electron_browser_app, Initialize)
