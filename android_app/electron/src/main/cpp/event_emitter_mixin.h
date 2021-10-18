#ifndef ANDROID_APP_EVENT_EMITTER_MIXIN_H
#define ANDROID_APP_EVENT_EMITTER_MIXIN_H

#include "event_emitter.h"
#include "javascript_environment.h"
#include "event_emitter_caller.h"
#include "object_template_builder.h"
#include "locker.h"

namespace gin_helper {
    template<typename T>
    class EventEmitterMixin {
    public:
        template<typename... Args>
        bool Emit(const char *name, Args &&... args) {
            v8::Isolate *isolate = JavascriptEnvironment::GetIsolate();
            v8::Locker locker(isolate);
            v8::HandleScope handle_scope(isolate);
            v8::Local<v8::Object> wrapper;
            if (!static_cast<T *>(this)->GetWrapper(isolate).ToLocal(&wrapper))
                return false;
            EmitEvent(isolate, wrapper, name, std::forward<Args>(args)...);
            return true;
        }

        template<typename... Args>
        v8::Local<v8::Object> EmitWithEvent(const char *name, v8::Local<v8::Object> eventObj, Args &&... args) {
            v8::Isolate *isolate = JavascriptEnvironment::GetIsolate();
            v8::Locker locker(isolate);
            v8::HandleScope handle_scope(isolate);
            v8::Local<v8::Object> wrapper;

            if (!static_cast<T *>(this)->GetWrapper(isolate).ToLocal(&wrapper))
                return eventObj;
            EmitEvent(isolate, wrapper, name, eventObj, std::forward<Args>(args)...);
            return eventObj;
        }

    protected:
        EventEmitterMixin() = default;

        virtual ObjectTemplateBuilder GetObjectTemplateBuilder(v8::Isolate *isolate) {
            // TODO Add cache for constructor like gin_helper::PerIsolateData
            v8::Local<v8::FunctionTemplate> constructor = v8::FunctionTemplate::New(isolate);
            constructor->SetClassName(
                    v8::String::NewFromUtf8(isolate, static_cast<T *>(this)->GetTypeName()));
            constructor->Inherit(internal::GetEventEmitterTemplate(isolate));

            return ObjectTemplateBuilder(isolate, static_cast<T *>(this)->GetTypeName(),
                                         constructor->InstanceTemplate());
        }
    };
}


#endif //ANDROID_APP_EVENT_EMITTER_MIXIN_H
