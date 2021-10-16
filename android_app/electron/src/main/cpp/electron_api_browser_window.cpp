#include "electron_api_browser_window.h"
#include "android_context.h"
#include "helper.h"
#include "node_includes.h"
#include "function_template.h"
#include "converter.h"

#include "libnode/include/node/uv.h"

#include <thread>

using namespace v8;

template<>
struct gin::Converter<BrowserWindow> {
    static bool FromV8(v8::Isolate *isolate,
                       v8::Local<v8::Value> val,
                       BrowserWindow *out) {
        return true;
    }
};

BrowserWindow::BrowserWindow(const FunctionCallbackInfo<Value> &info) {
    Isolate *isolate = info.GetIsolate();
    gin_helper::TrackableObject<BrowserWindow>::InitWith(isolate, info.Holder());

    Local<Object> properties = info[0]->ToObject(isolate);

    int id = android()->CreateWebContents(stringify(properties).c_str());
    web_contents_id_ = id;

    info.Holder()->Set(helper::StringToSymbol(isolate, "id"), Int32::New(isolate, web_contents_id_));
}

void BrowserWindow::LoadURL(const std::string &url) {
    android()->CommandToWebContents(web_contents_id_, __func__, url.c_str());
}

gin_helper::ObjectTemplateBuilder BrowserWindow::GetObjectTemplateBuilder(v8::Isolate *isolate) {
    return EventEmitterMixin::GetObjectTemplateBuilder(isolate);
}

const char *BrowserWindow::GetTypeName() {
    return "BrowserWindow";
}

void BrowserWindow::BuildPrototype(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> prototype) {
    prototype->SetClassName(helper::StringToSymbol(isolate, "BrowserWindow"));
    NODE_SET_PROTOTYPE_METHOD(prototype, "loadURL", [](const FunctionCallbackInfo<Value> &info) {
        auto self = convertData<BrowserWindow>(info);

        if (!info[0]->IsString()) {
            return;
        }

        self->LoadURL(helper::V8ToString(info.GetIsolate(), info[0]));
    });
}

namespace {

    void BrowserWindowConstructor(const FunctionCallbackInfo<Value> &info) {
        new BrowserWindow(info);
    }

    void Initialize(v8::Local<v8::Object> exports,
                    v8::Local<v8::Value> unused,
                    v8::Local<v8::Context> context) {
        Isolate *isolate = context->GetIsolate();

        Local<FunctionTemplate> tmpl = FunctionTemplate::New(isolate, BrowserWindowConstructor);
        BrowserWindow::BuildPrototype(isolate, tmpl);

        exports->Set(
                String::NewFromUtf8(isolate, "BrowserWindow"),
                tmpl->GetFunction(context).ToLocalChecked()
        );
    }

}

NODE_LINKED_MODULE_CONTEXT_AWARE(electron_browser_window, Initialize)