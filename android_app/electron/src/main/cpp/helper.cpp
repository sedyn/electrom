#include "helper.h"

using namespace v8;

std::string stringify(Local <Object> obj) {
    Isolate* isolate = obj->GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    Local<String> json = v8::JSON::Stringify(context, obj).ToLocalChecked();
    return std::string(*String::Utf8Value(isolate, json));
}
