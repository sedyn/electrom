#ifndef ANDROID_APP_ANDROID_CONTEXT_H
#define ANDROID_APP_ANDROID_CONTEXT_H

#include <jni.h>

#include "libnode/include/node/node.h"

class AndroidContext {
public:
    int CreateWebContents(int id, const char *propertiesJson) const;

    void CommandToWebContents(const int id, const char *command, const char *argument) const;

    void ReplyToIpcRenderer(const char* track_id, const char* channel, const char* response) const;

    static void Initialize(JNIEnv *env, jobject obj);

private:
    JNIEnv *env_;
};

/**
 * This context can only be used in MainProcess thread. If you use this context in other thread, it will cause 'zygote: runtime.cc:492' error
 * @return AndroidContext of MainProcess thread
 */
AndroidContext *android();

/**
 * AndroidContext* provides env of attached thread instead of MainProcess thread
 */
typedef void (*AndroidThread)(const AndroidContext *, void *data);

void RequestThread(AndroidThread func, void *data);

JNIEnv *AttachCurrentThread();

void DetachCurrentThread();

/**
* This method will be called in Embed Thread.
*/
void registerNextTick(JNIEnv *env);

#endif //ANDROID_APP_ANDROID_CONTEXT_H
