#ifndef ANDROID_APP_HANDLE_H
#define ANDROID_APP_HANDLE_H

#include "libnode/include/node/node.h"

namespace gin {

    template<typename T>
    class Handle {
    public:
        Handle() : object_(nullptr) {}

        Handle(v8::Local<v8::Value> wrapper, T *object)
                : wrapper_(wrapper),
                  object_(object) {
        }

        bool IsEmpty() const { return !object_; }

        void Clear() {
            wrapper_.Clear();
            object_ = NULL;
        }

        T *operator->() const { return object_; }

        v8::Local<v8::Value> ToV8() const { return wrapper_; }

        T *get() const { return object_; }

    private:
        v8::Local<v8::Value> wrapper_;
        T *object_ = NULL;
    };

    template<typename T>
    gin::Handle<T> CreateHandle(v8::Isolate* isolate, T* object) {
        v8::Local<v8::Object> wrapper;
        if (!object->GetWrapper(isolate).ToLocal(&wrapper) || wrapper.IsEmpty())
            return gin::Handle<T>();
        return gin::Handle<T>(wrapper, object);
    }
}

#endif // ANDROID_APP_HANDLE_H
