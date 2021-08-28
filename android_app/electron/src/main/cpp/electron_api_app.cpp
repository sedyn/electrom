#include "electron_api_app.h"

using namespace v8;

ElectronApp *app() {
    static auto *electronApp = new ElectronApp;
    return electronApp;
}

void ElectronApp::init(v8::Local<v8::Object> electron, Local<Object> eventEmitter) {
    if (!app.IsEmpty()) {
        return;
    }

    Isolate *isolate = electron->GetIsolate();

    auto *names = new Local<Name>[]{
            String::NewFromUtf8(isolate, "name")
    };
    auto *values = new Local<Value>[]{
            String::NewFromUtf8(isolate, "sample")
    };

    app = Object::New(isolate, eventEmitter, names, values, 1);

    electron->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "app"), app).Check();
}

void ElectronApp::Emit(const std::string &type, int argc, std::string *argv) const {
    Isolate *isolate = app->GetIsolate();

    Local<Function> emit = app->Get(String::NewFromUtf8(isolate, "emit")).As<Function>();

    std::vector<Local<Value>> arguments(argc + 1);
    arguments.emplace_back(String::NewFromUtf8(isolate, type.c_str()));
//    arguments.insert(arguments.begin() + 1, argv, argv + argc);

    Local<Value> called = emit->Call(isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(), arguments.size(), arguments.data()).ToLocalChecked();
}
