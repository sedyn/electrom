#ifndef ANDROID_APP_ELECTRON_API_APP_H
#define ANDROID_APP_ELECTRON_API_APP_H

#include "node_includes.h"
#include "event_emitter_mixin.h"

class ElectronApp : EventEmitterMixin<ElectronApp> {
public:
    v8::Local<v8::Object> Create(v8::Isolate* isolate);

    const char* GetTypeName() {
        return "app";
    }

    v8::Local<v8::FunctionTemplate> GetObjectTemplate(v8::Isolate* isolate);
};

ElectronApp* app();

#endif //ANDROID_APP_ELECTRON_API_APP_H
