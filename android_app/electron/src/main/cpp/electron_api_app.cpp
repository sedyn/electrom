#include "electron_api_app.h"

using namespace v8;

ElectronApp *app() {
    static auto *electronApp = new ElectronApp;
    return electronApp;
}

v8::Local<v8::FunctionTemplate> ElectronApp::GetObjectTemplate(v8::Isolate *isolate) {
    Local<FunctionTemplate> tmpl = EventEmitterMixin<ElectronApp>::GetObjectTemplate(isolate);
    return tmpl;
}

v8::Local<v8::Object> ElectronApp::Create(v8::Isolate* isolate) {

}

namespace {

    void Initialize(v8::Local<v8::Object> exports,
                    v8::Local<v8::Value> unused,
                    v8::Local<v8::Context> context) {
        Isolate *isolate = context->GetIsolate();
    }

}

NODE_LINKED_MODULE_CONTEXT_AWARE(electron_browser_app, Initialize)
