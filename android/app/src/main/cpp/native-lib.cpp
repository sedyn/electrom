#include <jni.h>
#include <string>
#include "include/node/node.h"

extern "C" JNIEXPORT jint JNICALL
Java_com_electrom_node_process_MainProcess_startMainModule(
        JNIEnv *env,
        jobject obj,
        jobjectArray arguments) {
    jclass cls = env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(cls, "startRendererProcess", "()V");
    if (mid == nullptr) {
        return 0;
    }
    env->CallVoidMethod(obj, mid);

    return 0;
}
