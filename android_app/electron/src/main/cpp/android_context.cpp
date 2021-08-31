#include <thread>
#include "android_context.h"

using namespace v8;

JavaVM *globalVM = nullptr;
AndroidContext *androidContext = nullptr;
jobject globalRef = nullptr;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    globalVM = vm;
    return JNI_VERSION_1_6;
}

AndroidContext *android() {
    return androidContext;
}

void InitAndroidContext(JNIEnv *env, jobject obj) {
    if (androidContext != nullptr) {
        return;
    }

    androidContext = new AndroidContext;
    androidContext->env = env;
    androidContext->obj = obj;
    globalRef = env->NewGlobalRef(obj);
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

void internalThread(AndroidThread func, void *data) {
    JNIEnv *env = nullptr;
    globalVM->AttachCurrentThread(&env, nullptr);
    if (env == nullptr) {
        globalVM->DetachCurrentThread();
        return;
    }

    auto internalAndroidContext = new AndroidContext;
    internalAndroidContext->env = env;
    internalAndroidContext->obj = globalRef;

    func(internalAndroidContext, data);
    free(internalAndroidContext);

    globalVM->DetachCurrentThread();
}

void RequestThread(AndroidThread func, void *data) {
    // https://stackoverflow.com/questions/42066700/ndk-c-stdthread-abort-crash-on-join
    std::thread(internalThread, func, data).detach();
}
