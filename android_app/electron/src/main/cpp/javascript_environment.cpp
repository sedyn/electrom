#include "javascript_environment.h"

using namespace v8;
using node::ArrayBufferAllocator;

Isolate *g_isolate;

JavascriptEnvironment::JavascriptEnvironment(uv_loop_t *event_loop)
    : isolate_(Initialize(event_loop)),
      locker_(isolate_){
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
    V8::Initialize();

    std::shared_ptr<ArrayBufferAllocator> allocator = ArrayBufferAllocator::Create();

    Isolate *isolate = NewIsolate(allocator.get(), event_loop, platform_);

    platform_->RegisterIsolate(isolate, event_loop);

    g_isolate = isolate;

    return isolate;
}
