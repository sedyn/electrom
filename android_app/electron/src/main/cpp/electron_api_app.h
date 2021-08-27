#ifndef ANDROID_APP_ELECTRON_API_APP_H
#define ANDROID_APP_ELECTRON_API_APP_H

#include "libnode/include/node/node.h"

class ElectronApp {
private:
public:
    v8::Local<v8::Object> app;

    void init(v8::Isolate *isolate, v8::Local<v8::Object> eventEmitter);
    void Emit(const std::string& type, int argc, std::string* argv) const;
};

void RegisterApp(v8::Isolate *isolate, v8::Local<v8::Object> module);

ElectronApp* get();

#endif //ANDROID_APP_ELECTRON_API_APP_H
