#ifndef ANDROID_APP_OBJECT_TEMPLATE_BUILDER_H
#define ANDROID_APP_OBJECT_TEMPLATE_BUILDER_H

#include "node_includes.h"

namespace gin_helper {
    class ObjectTemplateBuilder {
    public:
        ObjectTemplateBuilder(v8::Isolate *isolate, const char *type_name);

        ObjectTemplateBuilder(v8::Isolate *isolate, const char *type_name, v8::Local<v8::ObjectTemplate> tmpl);

        ~ObjectTemplateBuilder();

        v8::Local<v8::ObjectTemplate> Build();

        ObjectTemplateBuilder& SetMethod(const char* name, v8::FunctionCallback callback) {
            return SetImpl(name, v8::FunctionTemplate::New(isolate_, callback));
        }

    private:
        const char *type_name_ = nullptr;
        v8::Isolate *isolate_;
        v8::Local<v8::ObjectTemplate> template_;

        ObjectTemplateBuilder &SetImpl(const char *name, v8::Local<v8::Data> val);
    };
}

#endif //ANDROID_APP_OBJECT_TEMPLATE_BUILDER_H
