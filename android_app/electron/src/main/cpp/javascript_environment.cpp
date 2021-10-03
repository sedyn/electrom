#include "javascript_environment.h"

using namespace v8;
using node::ArrayBufferAllocator;

Isolate *g_isolate = nullptr;
ArrayBufferAllocator *g_array_buffer_allocator = nullptr;

JavascriptEnvironment::JavascriptEnvironment(uv_loop_t *event_loop)
        : isolate_(Initialize(event_loop)),
          locker_(isolate_) {
    isolate_->Enter();
    HandleScope scope(isolate_);
    auto context = node::NewContext(isolate_);
    context_ = Global<Context>(isolate_, context);
    context->Enter();
}

v8::Isolate *JavascriptEnvironment::Initialize(uv_loop_t *event_loop) {
    node::tracing::TracingController *tracing_controller = nullptr;
    platform_ = node::CreatePlatform(2, tracing_controller);

    V8::InitializePlatform(platform_);
    // https://chromium.googlesource.com/chromium/src/+/refs/heads/main/gin/v8_initializer.cc
    V8::Initialize();

    g_array_buffer_allocator = node::CreateArrayBufferAllocator();

    Isolate *isolate = Isolate::Allocate();

    platform_->RegisterIsolate(isolate, event_loop);

    g_isolate = isolate;

    // https://chromium.googlesource.com/chromium/src/+/refs/heads/main/gin/isolate_holder.cc
    Isolate::CreateParams params;
    params.array_buffer_allocator = g_array_buffer_allocator;
    params.only_terminate_in_safe_scope = true;

    Isolate::Initialize(isolate, params);

    return isolate;
}
