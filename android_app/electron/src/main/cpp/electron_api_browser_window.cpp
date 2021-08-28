#include "electron_api_browser_window.h"
#include "android_context.h"
#include "helper.h"

using namespace v8;

void BrowserWindowConstructor(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    Local<Object> properties = args[0]->ToObject(isolate);
    android()->StartRendererProcess(stringify(properties).c_str());
}

void loadUrl(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    String::Utf8Value i(isolate, args[0]->ToString(isolate));
}

void RegisterBrowserWindow(Local<Object> electron, Local<Object> eventEmitter) {
    Isolate *isolate = electron->GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    Local<String> browserWindowName = String::NewFromUtf8(isolate, "BrowserWindow");

    Local<FunctionTemplate> BrowserWindowTemplate = FunctionTemplate::New(isolate, &BrowserWindowConstructor);
    BrowserWindowTemplate->SetClassName(browserWindowName);
    NODE_SET_PROTOTYPE_METHOD(BrowserWindowTemplate, "loadURL", &loadUrl);

    Local<Function> BrowserWindow = BrowserWindowTemplate->GetFunction(context).ToLocalChecked();
    BrowserWindow->Set(String::NewFromUtf8(isolate, "id"),  Number::New(isolate, 1));
    BrowserWindow->SetName(browserWindowName);

    // event_emitter_mixin.cc 참고
    Local<Value> BrowserWindowPrototype = BrowserWindow->Get(context, String::NewFromUtf8(isolate, "prototype")).ToLocalChecked();
    BrowserWindowPrototype.As<Object>()->SetPrototype(context, eventEmitter).Check();

    electron->Set(context, browserWindowName, BrowserWindow).Check();
}
