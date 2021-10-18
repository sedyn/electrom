#include "electron_api_ipc_main.h"

using namespace v8;

IpcMain *IpcMain::Get() {
    static auto ipcMain = new IpcMain();
    return ipcMain;
}

void IpcMain::Init(v8::Isolate *isolate, v8::Local<v8::Object> wrapper) {
    gin_helper::Wrappable<IpcMain>::Init(isolate, wrapper);
}

std::string IpcMain::HandleSyncEvent(const char *event, const char *data) {
    Isolate *isolate = JavascriptEnvironment::GetIsolate();
    v8::Locker locker(isolate);
    v8::HandleScope handle_scope(isolate);
    Local<Object> eventObj = Object::New(isolate);
    EmitWithEvent(event, eventObj, data);
    Local<Value> returnVal = eventObj->Get(
            isolate->GetCurrentContext(),
            helper::StringToSymbol(isolate, "returnValue")
    ).ToLocalChecked();
    if (returnVal->IsNull() || !returnVal->IsString()) {
        return nullptr;
    }

    return std::string(*String::Utf8Value(isolate, returnVal.As<String>()));
}

const char *IpcMain::GetTypeName() {
    return "IpcMain";
}

namespace {

    void InitializeIpcMain(const FunctionCallbackInfo<Value> &info) {
        IpcMain *ipcMain = IpcMain::Get();
        ipcMain->Init(info.GetIsolate(), info[0].As<v8::Object>());
    }

    void Initialize(v8::Local<v8::Object> exports,
                    v8::Local<v8::Value> unused,
                    v8::Local<v8::Context> context) {
        NODE_SET_METHOD(exports, "_init", InitializeIpcMain);
    }

}

NODE_LINKED_MODULE_CONTEXT_AWARE(electron_ipc_main, Initialize);