#ifndef ANDROID_APP_ANDROID_CONTEXT_H
#define ANDROID_APP_ANDROID_CONTEXT_H

#include <jni.h>

class AndroidContext {
private:
    JNIEnv *env;
    jobject obj;
public:
    const char *StartRendererProcess(const char *propertiesJson) const;

    void CommandToRendererProcess(const char *command, const char *argument) const;

    void init(JNIEnv *_env, jobject _obj);
};

AndroidContext *android();

#endif //ANDROID_APP_ANDROID_CONTEXT_H
