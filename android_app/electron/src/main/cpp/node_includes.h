#ifndef ANDROID_APP_NODE_INCLUDES_H_
#define ANDROID_APP_NODE_INCLUDES_H_

#include "libnode/include/node/node_binding.h"

#define NODE_LINKED_MODULE_CONTEXT_AWARE(modname, regfunc) \
  NODE_MODULE_CONTEXT_AWARE_CPP(modname, regfunc, nullptr, NM_F_LINKED)

namespace helper {

    static v8::Local<v8::String> StringToSymbol(v8::Isolate *isolate, const char *val) {
        return v8::String::NewFromUtf8(isolate, val, v8::NewStringType::kInternalized).ToLocalChecked();
    }

    static std::string V8ToString(v8::Isolate *isolate, v8::Local<v8::Value> val) {
        return std::string(*v8::String::Utf8Value(isolate, val));
    }

}

#endif
