#include "event_emitter.h"

v8::Eternal<v8::FunctionTemplate> event_emitter = v8::Eternal<v8::FunctionTemplate>();

v8::Global<v8::Object> *GetEventEmitterPrototypeReference() {
    static auto *event_emitter_prototype = new v8::Global<v8::Object>;
    return event_emitter_prototype;
}

void SetEventEmitterPrototype(const v8::FunctionCallbackInfo<v8::Value> &args) {
    v8::Isolate *isolate = args.GetIsolate();
    GetEventEmitterPrototypeReference()->Reset(isolate, args[0].As<v8::Object>());
}

namespace gin_helper::internal {

    v8::Local<v8::FunctionTemplate> GetEventEmitterTemplate(v8::Isolate *isolate) {
        if (event_emitter.IsEmpty()) {
            v8::Local<v8::FunctionTemplate> tmpl = v8::FunctionTemplate::New(isolate);
            v8::Local<v8::Context> context = isolate->GetCurrentContext();
            v8::Local<v8::Function> func = tmpl->GetFunction(context).ToLocalChecked();

            v8::Local<v8::Object> event_emitter_prototype = GetEventEmitterPrototypeReference()->Get(
                    isolate);

            v8::Local<v8::Value> func_prototype;
            if (!func->Get(context, v8::String::NewFromUtf8(isolate, "prototype")).ToLocal(
                    &func_prototype)) {

            }

            func_prototype.As<v8::Object>()->SetPrototype(context,
                                                          event_emitter_prototype).ToChecked();

            event_emitter.Set(isolate, tmpl);
        }

        return event_emitter.Get(isolate);
    }

}

namespace {

    void Initialize(v8::Local<v8::Object> exports,
                    v8::Local<v8::Value> unused,
                    v8::Local<v8::Context> context) {
        NODE_SET_METHOD(exports, "setEventEmitterPrototype", SetEventEmitterPrototype);
    }

}

NODE_LINKED_MODULE_CONTEXT_AWARE(electron_browser_event_emitter, Initialize)
