#ifndef ANDROID_APP_ANDROID_CONTEXT_H
#define ANDROID_APP_ANDROID_CONTEXT_H

#include <jni.h>
#include "libnode/include/node/node.h"

class AndroidContext {
public:
    JNIEnv *env;
    jobject obj;

    const char *StartRendererProcess(const char *propertiesJson) const;

    void CommandToRendererProcess(const char *command, const char *argument) const;
};

void InitAndroidContext(JNIEnv* env, jobject obj);

/**
 * This context can only be used in MainProcess thread. If you use this context in other thread, it will cause 'zygote: runtime.cc:492' error
 * @return AndroidContext of MainProcess thread
 */
AndroidContext *android();

/**
 * AndroidContext* provides env of attached thread instead of MainProcess thread
 */
typedef void (*AndroidThread)(const AndroidContext*, void* data);

void RequestThread(AndroidThread func, void* data);

#endif //ANDROID_APP_ANDROID_CONTEXT_H
