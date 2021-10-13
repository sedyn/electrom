#ifndef ANDROID_APP_EVENT_EMITTER_MIXIN_H
#define ANDROID_APP_EVENT_EMITTER_MIXIN_H

#include "event_emitter.h"

template <typename T>
class EventEmitterMixin {
protected:
    EventEmitterMixin() = default;

    virtual v8::Local<v8::FunctionTemplate> GetObjectTemplate(v8::Isolate* isolate) {
        // TODO Add cache for constructor like gin::PerIsolateData
        v8::Local<v8::FunctionTemplate> constructor = v8::FunctionTemplate::New(isolate);
        constructor->SetClassName(v8::String::NewFromUtf8(isolate, static_cast<T*>(this)->GetTypeName()));
        constructor->Inherit(internal::GetEventEmitterTemplate(isolate));
        return constructor;
    }
};

#endif //ANDROID_APP_EVENT_EMITTER_MIXIN_H
