#ifndef ANDROID_APP_ELECTRON_API_IPC_MAIN_H
#define ANDROID_APP_ELECTRON_API_IPC_MAIN_H

#include "node_includes.h"
#include "wrappable.h"
#include "event_emitter_mixin.h"

class IpcMain : public gin_helper::Wrappable<IpcMain>,
                public gin_helper::EventEmitterMixin<IpcMain> {

public:
    static IpcMain *Get();

    void Init(v8::Isolate *isolate, v8::Local<v8::Object> wrapper);

    std::string HandleSyncEvent(const char *event, const char *data);

    const char* GetTypeName();

};

#endif //ANDROID_APP_ELECTRON_API_IPC_MAIN_H
