#ifndef ANDROID_APP_EVENT_EMITTER_CALLER_H
#define ANDROID_APP_EVENT_EMITTER_CALLER_H

#include "libnode/include/node/node.h"
#include <vector>

// shell/common.gin_helper/event_emitter_caller.h
namespace internal {

    using ValueVector = std::vector<v8::Local<v8::Value>>;

    v8::Local<v8::Value> CallMethodWithArgs(v8::Isolate* isolate,
                                            v8::Local<v8::Object> obj,
                                            const char* method,
                                            ValueVector* args);

}

//namespace gin_helper {
//
//    v8::Local<v8::Value> EmitEvent(v8::Isolate* isolate,
//                                   v8::Local<v8::Object> obj,
//                                   const char* name,
//                                   const internal::ValueVector& args) {
//        internal::ValueVector concatenated_args = {v8::String::NewFromUtf8(isolate, name)};
//        concatenated_args.reserve(1 + args.size());
//        concatenated_args.insert(concatenated_args.end(), args.begin(), args.end());
//        return internal::CallMethodWithArgs(isolate, obj, "emit", &concatenated_args);
//    }
//
//
//}


#endif //ANDROID_APP_EVENT_EMITTER_CALLER_H
