#include "electron_api_app.h"

using namespace v8;

ElectronApp *get() {
    static auto *electronApp = new ElectronApp;
    return electronApp;
}

void ElectronApp::init(Isolate *isolate, Local<Object> eventEmitter) {
    if (!app.IsEmpty()) {
        return;
    }

    auto *names = new Local<Name>[]{
            String::NewFromUtf8(isolate, "name")
    };
    auto *values = new Local<Value>[]{
            String::NewFromUtf8(isolate, "sample")
    };

    app = Object::New(isolate, eventEmitter, names, values, 1);
}

void ElectronApp::Emit(const std::string& type, int argc, std::string *argv) const {
    Isolate *isolate = app->GetIsolate();

    Local<Function> emit = app->Get(String::NewFromUtf8(isolate, "emit")).As<Function>();

    std::vector<Local<Value>> arguments(argc + 1);
    arguments.emplace_back(String::NewFromUtf8(isolate, type.c_str()));
//    arguments.insert(arguments.begin() + 1, argv, argv + argc);

    Local<Value> called = emit->Call(isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(), arguments.size(), arguments.data()).ToLocalChecked();
}

void CreateAppInstance(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();

    Local<Object> eventEmitter = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();

    auto app = get();
    app->init(isolate, eventEmitter);

    args.GetReturnValue().Set(app->app);
}

void RegisterApp(Isolate *isolate, Local<Object> module) {
    NODE_SET_METHOD(module, "initApp", &CreateAppInstance);
}