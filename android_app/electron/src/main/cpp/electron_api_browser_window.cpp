#include "electron_api_browser_window.h"
#include "android_context.h"
#include "helper.h"
#include "node_includes.h"
#include "function_template.h"
#include "converter.h"

#include "libnode/include/node/uv.h"

#include <thread>

using namespace v8;

// shell/common/gin_helper/wrappable.cc:internal:FromV8Impl
bool gin::Converter<BrowserWindow *>::FromV8(v8::Isolate *isolate,
                                             v8::Local<v8::Value> val,
                                             BrowserWindow **out) {
    if (!val->IsObject()) {
        *out = nullptr;
        return false;
    }

    Local<Object> obj = val.As<Object>();
    if (obj->InternalFieldCount() != 1) {
        *out = nullptr;
        return false;
    }

    *out = static_cast<BrowserWindow *>(obj->GetAlignedPointerFromInternalField(0));
    return true;
}

BrowserWindow::BrowserWindow(const FunctionCallbackInfo<Value> &info) {
    Isolate *isolate = info.GetIsolate();
    Local<Object> holder = info.Holder();

    gin_helper::TrackableObject<BrowserWindow>::InitWith(isolate, holder);

    Local<Object> properties = info[0]->ToObject(isolate);

    android()->CreateWebContents(stringify(properties).c_str());
    web_contents_id_ = weak_map_id_;

    holder->SetAccessorProperty(
            helper::StringToSymbol(isolate, "id"),
            Function::New(isolate->GetCurrentContext(), [](const FunctionCallbackInfo<Value> &info) {
                Isolate *innerIsolate = info.GetIsolate();
                Local<Object> innerHolder = info.Holder();
                BrowserWindow *browserWindow = nullptr;
                gin::ConvertFromV8(innerIsolate, innerHolder, &browserWindow);
                info.GetReturnValue().Set(browserWindow->weak_map_id_);
            }).ToLocalChecked()
    );
}

const char *BrowserWindow::GetTypeName() {
    return "BrowserWindow";
}

void BrowserWindow::LoadURL(const std::string &url) {
    android()->CommandToWebContents(web_contents_id_, __func__, url.c_str());
    Emit("ready-to-show");
}

void BrowserWindow::Show() {
    android()->CommandToWebContents(web_contents_id_, __func__, nullptr);
}

void BrowserWindow::BuildPrototype(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> prototype) {
    prototype->SetClassName(helper::StringToSymbol(isolate, "BrowserWindow"));
    prototype->Inherit(gin_helper::internal::GetEventEmitterTemplate(isolate));

    NODE_SET_PROTOTYPE_METHOD(prototype, "loadURL", [](const FunctionCallbackInfo<Value> &info) {
        Isolate *isolate = info.GetIsolate();
        int id = info.Holder()->Get(helper::StringToSymbol(isolate, "id")).As<Int32>()->Value();
        BrowserWindow *self = FromWeakMapID(isolate, id);

        if (!info[0]->IsString()) {
            return;
        }

        self->LoadURL(helper::V8ToString(info.GetIsolate(), info[0]));
    });

    NODE_SET_PROTOTYPE_METHOD(prototype, "show", [](const FunctionCallbackInfo<Value> &info) {
        Isolate *isolate = info.GetIsolate();
        int id = info.Holder()->Get(helper::StringToSymbol(isolate, "id")).As<Int32>()->Value();
        BrowserWindow *self = FromWeakMapID(isolate, id);

        self->Show();
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
        tmpl->InstanceTemplate()->SetInternalFieldCount(1);
        BrowserWindow::BuildPrototype(isolate, tmpl);

        exports->Set(
                String::NewFromUtf8(isolate, "BrowserWindow"),
                tmpl->GetFunction(context).ToLocalChecked()
        );
    }

}

NODE_LINKED_MODULE_CONTEXT_AWARE(electron_browser_window, Initialize)