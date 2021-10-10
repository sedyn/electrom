#ifndef ANDROID_APP_EVENT_H
#define ANDROID_APP_EVENT_H

#include "node_includes.h"

v8::Global<v8::Object>* GetEventEmitterPrototypeReference();

namespace internal {
    v8::Local<v8::FunctionTemplate> GetEventEmitterTemplate(v8::Isolate * isolate);
}

#endif //ANDROID_APP_EVENT_H
