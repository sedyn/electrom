#include "android_context.h"

AndroidContext *android() {
    static auto *android_context = new AndroidContext;
    return android_context;
}

void AndroidContext::init(JNIEnv *_env, jobject _obj) {
    env = _env;
    obj = _obj;
}

void AndroidContext::CommandToRendererProcess(const char *command, const char *argument) const {
    jclass cls = env->GetObjectClass(obj);
    jmethodID mId = env->GetMethodID(cls, "commandToRendererProcess", "(Ljava/lang/String;Ljava/lang/String;)V");

    jstring j_command = env->NewStringUTF(command);
    if (argument != nullptr) {
        jstring j_argument = env->NewStringUTF(argument);
        env->CallVoidMethod(obj, mId, j_command, j_argument);
    } else {
        env->CallVoidMethod(obj, mId, j_command, NULL);
    }
}

const char *AndroidContext::StartRendererProcess(const char *propertiesJson) const {
    jstring properties = env->NewStringUTF(propertiesJson);

    jclass cls = env->GetObjectClass(obj);
    jmethodID mId = env->GetMethodID(cls, "startRendererProcess", "(Ljava/lang/String;)Ljava/lang/String;");
    auto returnVal = (jstring) env->CallObjectMethod(obj, mId, properties);

    const char *rendererId = env->GetStringUTFChars(returnVal, nullptr);
    return rendererId;
}