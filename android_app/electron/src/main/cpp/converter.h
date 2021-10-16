#ifndef ANDROID_APP_CONVERTER_H
#define ANDROID_APP_CONVERTER_H

#include "libnode/include/node/v8.h"

namespace gin {

    template<typename T, typename Enable = void>
    struct Converter {
    };

    template<typename T>
    bool ConvertFromV8(v8::Isolate isolate, v8::Local<v8::Value> input, T *result) {
        return Converter<T>::FromV8(isolate, input, result);
    }
}

#endif //ANDROID_APP_CONVERTER_H
