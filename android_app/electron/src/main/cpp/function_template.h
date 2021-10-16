#ifndef ANDROID_APP_FUNCTION_TEMPLATE_H
#define ANDROID_APP_FUNCTION_TEMPLATE_H

#include "libnode/include/node/node.h"

template<typename T>
inline T *convertData(const v8::FunctionCallbackInfo<v8::Value> &info) {
    v8::Local<v8::External> self = info.Data().As<v8::External>();
    T *castedSelf = static_cast<T *>(self->Value());
    return castedSelf;
}

#endif //ANDROID_APP_FUNCTION_TEMPLATE_H
