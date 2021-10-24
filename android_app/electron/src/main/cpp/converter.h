#ifndef ANDROID_APP_CONVERTER_H
#define ANDROID_APP_CONVERTER_H

#include "libnode/include/node/v8.h"

namespace gin {
    template<typename T, typename Enable = void>
    struct Converter {
    };

    template<typename T>
    bool ConvertFromV8(v8::Isolate *isolate, v8::Local<v8::Value> input, T *result) {
        return Converter<T>::FromV8(isolate, input, result);
    }

    template<typename T>
    v8::Local<v8::Value> ConvertToV8(v8::Isolate *isolate, T &&input) {
        return Converter<typename std::remove_reference<T>::type>::ToV8(isolate, std::move(input));
    }

    template<>
    struct Converter<const char *> {
        static v8::Local<v8::Value> ToV8(v8::Isolate *isolate, const char *val) {
            if (val == nullptr) {
                return v8::Undefined(isolate);
            } else {
                return v8::String::NewFromUtf8(isolate, val, v8::NewStringType::kNormal).ToLocalChecked();
            }
        }
    };

    template<>
    struct Converter<v8::Local<v8::Object>> {
        static v8::Local<v8::Value> ToV8(v8::Isolate *isolate, v8::Local<v8::Object> val) {
            return val;
        }
    };
}

#endif //ANDROID_APP_CONVERTER_H
