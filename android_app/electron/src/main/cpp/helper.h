#ifndef ANDROID_APP_HELPER_H
#define ANDROID_APP_HELPER_H

#include "libnode/include/node/node.h"
#include <string>

std::string stringify(v8::Isolate *isolate, v8::Local<v8::Object> obj);

#endif //ANDROID_APP_HELPER_H
