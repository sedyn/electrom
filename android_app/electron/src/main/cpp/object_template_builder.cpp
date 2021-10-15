#include "object_template_builder.h"

namespace gin {
    ObjectTemplateBuilder::ObjectTemplateBuilder(v8::Isolate *isolate, const char *type_name)
            : ObjectTemplateBuilder::ObjectTemplateBuilder(isolate, type_name, v8::ObjectTemplate::New(isolate)) {}

    ObjectTemplateBuilder::ObjectTemplateBuilder(v8::Isolate *isolate, const char *type_name, v8::Local<v8::ObjectTemplate> tmpl)
            : isolate_(isolate), type_name_(type_name), template_(tmpl) {}

    ObjectTemplateBuilder::~ObjectTemplateBuilder() = default;

    v8::Local<v8::ObjectTemplate> ObjectTemplateBuilder::Build() {
        v8::Local<v8::ObjectTemplate> result = template_;
        template_.Clear();
        return result;
    }

    ObjectTemplateBuilder &ObjectTemplateBuilder::SetImpl(const char *name, v8::Local<v8::Data> val) {
        template_->Set(StringToSymbol(isolate_, name), val);
        return *this;
    }
}