#ifndef ANDROID_APP_JAVASCRIPT_ENVIRONMENT_H
#define ANDROID_APP_JAVASCRIPT_ENVIRONMENT_H

#include "libnode/include/node/uv.h"
#include "libnode/include/node/node.h"

class JavascriptEnvironment {
public:
    explicit JavascriptEnvironment(uv_loop_t* event_loop);

    node::MultiIsolatePlatform* platform() const { return platform_; }
    v8::Isolate* isolate() const { return isolate_; }
    v8::Local<v8::Context> context() const {
        return v8::Local<v8::Context>::New(isolate_, context_);
    }

    static v8::Isolate* GetIsolate();
private:
    v8::Isolate* Initialize(uv_loop_t* event_loop);

    node::MultiIsolatePlatform* platform_;

    v8::Isolate* isolate_;
    v8::Locker locker_;
    v8::Global<v8::Context> context_;
};


#endif //ANDROID_APP_JAVASCRIPT_ENVIRONMENT_H
