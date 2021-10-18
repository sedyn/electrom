#include "event_emitter_caller.h"
#include "libnode/include/node/node.h"

v8::Local<v8::Value> gin_helper::internal::CallMethodWithArgs(v8::Isolate *isolate,
                                                              v8::Local<v8::Object> obj,
                                                              const char *method,
                                                              ValueVector *args) {
    v8::MaybeLocal<v8::Value> ret = node::MakeCallback(
            isolate, obj, method, args->size(), args->data(), {0, 0});
    v8::Local<v8::Value> localRet;
    if (ret.ToLocal(&localRet)) {
        return localRet;
    }
    return v8::Boolean::New(isolate, false);
}