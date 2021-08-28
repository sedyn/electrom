#ifndef ANDROID_APP_ELECTRON_API_APP_H
#define ANDROID_APP_ELECTRON_API_APP_H

#include "libnode/include/node/node.h"

class ElectronApp {
private:
    v8::Local<v8::Object> app;
public:
    void init(v8::Local<v8::Object> electron, v8::Local<v8::Object> eventEmitter);
    void Emit(const std::string& type, int argc, std::string* argv) const;
};

ElectronApp* app();

#endif //ANDROID_APP_ELECTRON_API_APP_H
