#include "electron_api_browser_window.h"
#include "android_context.h"
#include "helper.h"

using namespace v8;

void BrowserWindowConstructor(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    Local<Object> properties = args[0]->ToObject(isolate);

    Local<Object> browserWindow = args.Holder();
    const char *processId = android()->StartRendererProcess(stringify(properties).c_str());

    browserWindow->Set(String::NewFromUtf8(isolate, "id"), String::NewFromUtf8(isolate, processId));
}

void loadURL(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    Local<String> url = args[0]->ToString(isolate);

    android()->CommandToRendererProcess(__func__, std::string(*String::Utf8Value(isolate, url)).c_str());

//    auto resolver = Promise::Resolver::New(context).ToLocalChecked();
//    args.GetReturnValue().Set(resolver->GetPromise());
}

void show(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    android()->CommandToRendererProcess(__func__, nullptr);
}

void RegisterBrowserWindow(Local<Object> electron, Local<Object> eventEmitter) {
    Isolate *isolate = electron->GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    Local<String> browserWindowName = String::NewFromUtf8(isolate, "BrowserWindow");

    Local<FunctionTemplate> BrowserWindowTemplate = FunctionTemplate::New(isolate, &BrowserWindowConstructor);
    BrowserWindowTemplate->SetClassName(browserWindowName);
    NODE_SET_PROTOTYPE_METHOD(BrowserWindowTemplate, "loadURL", &loadURL);
    NODE_SET_PROTOTYPE_METHOD(BrowserWindowTemplate, "show", &show);

    Local<Function> BrowserWindow = BrowserWindowTemplate->GetFunction(context).ToLocalChecked();
    BrowserWindow->SetName(browserWindowName);

    // event_emitter_mixin.cc 참고
    Local<Value> BrowserWindowPrototype = BrowserWindow->Get(context, String::NewFromUtf8(isolate, "prototype")).ToLocalChecked();
    BrowserWindowPrototype.As<Object>()->SetPrototype(context, eventEmitter).Check();

    electron->Set(context, browserWindowName, BrowserWindow).Check();
}
