#ifndef ANDROID_APP_EVENT_EMITTER_CALLER_H
#define ANDROID_APP_EVENT_EMITTER_CALLER_H

#include "converter.h"
#include <vector>

namespace gin_helper {

    // shell/common.gin_helper/event_emitter_caller.h
    namespace internal {

        using ValueVector = std::vector<v8::Local<v8::Value>>;

        v8::Local<v8::Value> CallMethodWithArgs(v8::Isolate *isolate,
                                                v8::Local<v8::Object> obj,
                                                const char *method,
                                                ValueVector *args);

    }

    template<typename ... Args>
    static v8::Local<v8::Value> EmitEvent(v8::Isolate *isolate,
                                          v8::Local<v8::Object> obj,
                                          const char *name,
                                          Args &&... args) {
        internal::ValueVector converted_args = {
                v8::String::NewFromUtf8(isolate, name),
                gin::ConvertToV8(isolate, std::forward<Args>(args))...,
        };
        return internal::CallMethodWithArgs(isolate, obj, "emit", &converted_args);
    }

}


#endif //ANDROID_APP_EVENT_EMITTER_CALLER_H
