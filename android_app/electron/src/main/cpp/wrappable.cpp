#include "wrappable.h"

namespace gin_helper {
    WrappableBase::WrappableBase() = default;

    WrappableBase::~WrappableBase() {
        wrapper_.Reset();
    }

    ObjectTemplateBuilder WrappableBase::GetObjectTemplateBuilder(v8::Isolate *isolate) {
        return ObjectTemplateBuilder(isolate, GetTypeName());
    }

    const char *WrappableBase::GetTypeName() {
        return nullptr;
    }

    void WrappableBase::FirstWeakCallback(const v8::WeakCallbackInfo<WrappableBase> &data) {
        WrappableBase *wrappable = data.GetParameter();
        wrappable->dead_ = true;
        wrappable->wrapper_.Reset();
        data.SetSecondPassCallback(SecondWeakCallback);
    }

    void WrappableBase::SecondWeakCallback(const v8::WeakCallbackInfo<WrappableBase> &data) {
        WrappableBase *wrappable = data.GetParameter();
        delete wrappable;
    }

    v8::MaybeLocal<v8::Object> gin_helper::WrappableBase::GetWrapperImpl(v8::Isolate *isolate) {
        if (!wrapper_.IsEmpty()) {
            return v8::MaybeLocal<v8::Object>(
                    v8::Local<v8::Object>::New(isolate, wrapper_));
        }

        if (dead_)
            return v8::MaybeLocal<v8::Object>();

        // TODO Add Cache here
        v8::Local<v8::ObjectTemplate> tmpl = GetObjectTemplateBuilder(isolate).Build();
        v8::Local<v8::Object> wrapper;

        if (!tmpl->NewInstance(isolate->GetCurrentContext()).ToLocal(&wrapper)) {
            delete this;
            return v8::MaybeLocal<v8::Object>(wrapper);
        }

        wrapper_.Reset(isolate, wrapper);
        wrapper_.SetWeak(this, FirstWeakCallback, v8::WeakCallbackType::kParameter);
        return v8::MaybeLocal<v8::Object>(wrapper);
    }

    void WrappableBase::InitWith(v8::Isolate *isolate, v8::Local<v8::Object> wrapper) {
        wrapper->SetAlignedPointerInInternalField(0, this);
        wrapper_.Reset(isolate, wrapper);
        wrapper_.SetWeak(this, FirstWeakCallback, v8::WeakCallbackType::kInternalFields);
    }
}
