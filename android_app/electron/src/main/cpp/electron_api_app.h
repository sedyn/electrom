#ifndef ANDROID_APP_ELECTRON_API_APP_H
#define ANDROID_APP_ELECTRON_API_APP_H

#include "node_includes.h"

#include "event_emitter_mixin.h"
#include "wrappable.h"
#include "handle.h"

class App : public gin_helper::Wrappable<App>,
            public gin_helper::EventEmitterMixin<App> {
public:
    static gin::Handle<App> Create(v8::Isolate *isolate);

    static App *Get();

    gin_helper::ObjectTemplateBuilder GetObjectTemplateBuilder(v8::Isolate *isolate) override;

    const char *GetTypeName() override;

    bool EmitReady();
};

#endif //ANDROID_APP_ELECTRON_API_APP_H
