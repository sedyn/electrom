#ifndef ANDROID_APP_ELECTRON_API_APP_H
#define ANDROID_APP_ELECTRON_API_APP_H

#include "node_includes.h"
#include "event_emitter_mixin.h"

class App : public EventEmitterMixin<App> {
public:
    const char *GetTypeName();
    v8::Local<v8::ObjectTemplate> GetObjectTemplate(v8::Isolate *isolate);
};

App *app();

#endif //ANDROID_APP_ELECTRON_API_APP_H
