#include "helper.h"

using v8::Local;
using v8::Object;
using v8::Isolate;
using v8::Context;
using v8::String;

std::string stringify(Isolate* isolate, Local <Object> obj) {
    Local<Context> context = isolate->GetCurrentContext();
    Local<String> json = v8::JSON::Stringify(context, obj).ToLocalChecked();
    return std::string(*String::Utf8Value(isolate, json));
}
