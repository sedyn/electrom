#include "android_context.h"

AndroidContext *android() {
    static auto *android_context = new AndroidContext;
    return android_context;
}

void AndroidContext::init(JNIEnv *_env, jobject _obj) {
    env = _env;
    obj = _obj;
}

void AndroidContext::StartRendererProcess(const char *propertiesJson) const {
    jstring jstr = env->NewStringUTF(propertiesJson);

    jclass cls = env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(cls, "startRendererProcess", "(Ljava/lang/String;)V");
    env->CallVoidMethod(obj, mid, jstr);
}