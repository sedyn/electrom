#ifndef ANDROID_APP_ELECTRON_API_BROWSER_WINDOW_H
#define ANDROID_APP_ELECTRON_API_BROWSER_WINDOW_H

#include "libnode/include/node/node.h"

void RegisterBrowserWindow(v8::Isolate *isolate, v8::Local<v8::Object> module);

#endif //ANDROID_APP_ELECTRON_API_BROWSER_WINDOW_H
