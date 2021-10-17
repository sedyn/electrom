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

void AndroidContext::Initialize(JNIEnv *env, jobject obj) {
    if (androidContext != nullptr) {
        return;
    }

    androidContext = new AndroidContext;
    androidContext->env_ = env;
    globalRef = env->NewGlobalRef(obj);
}

void AndroidContext::CommandToWebContents(const int id, const char *command, const char *argument) const {
    jclass cls = env_->GetObjectClass(globalRef);
    jmethodID mId = env_->GetMethodID(cls, "commandToWebContents", "(ILjava/lang/String;Ljava/lang/String;)V");
    jstring j_command = env_->NewStringUTF(command);
    if (argument != nullptr) {
        jstring j_argument = env_->NewStringUTF(argument);
        env_->CallVoidMethod(globalRef, mId, id, j_command, j_argument);
    } else {
        env_->CallVoidMethod(globalRef, mId, id, j_command, NULL);
    }
}

int AndroidContext::CreateWebContents(int id, const char *propertiesJson) const {
    jstring properties = env_->NewStringUTF(propertiesJson);
    jclass cls = env_->GetObjectClass(globalRef);
    jmethodID mId = env_->GetMethodID(cls, "createWebContents", "(ILjava/lang/String;)V");
    env_->CallVoidMethod(globalRef, mId, id, properties);
    return id;
}

void internalThread(AndroidThread func, void *data) {
    JNIEnv *env = nullptr;
    globalVM->AttachCurrentThread(&env, nullptr);
    if (env == nullptr) {
        globalVM->DetachCurrentThread();
        return;
    }

    auto internalAndroidContext = new AndroidContext;
//    internalAndroidContext->env = env;
//    internalAndroidContext->obj = globalRef;

    func(internalAndroidContext, data);
    free(internalAndroidContext);

    globalVM->DetachCurrentThread();
}

void RequestThread(AndroidThread func, void *data) {
    // https://stackoverflow.com/questions/42066700/ndk-c-stdthread-abort-crash-on-join
    std::thread(internalThread, func, data).detach();
}

JNIEnv *AttachCurrentThread() {
    JNIEnv *env = nullptr;
    globalVM->AttachCurrentThread(&env, nullptr);
    if (env == nullptr) {
        globalVM->DetachCurrentThread();
        return nullptr;
    }

    return env;
}

void DetachCurrentThread() {
    globalVM->DetachCurrentThread();
}

void AddTaskForMainLooper(JNIEnv *env) {
    jclass cls = env->GetObjectClass(globalRef);
    jmethodID mId = env->GetMethodID(cls, "addTask", "()V");
    env->CallVoidMethod(globalRef, mId);
}
