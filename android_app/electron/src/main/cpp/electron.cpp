#include <jni.h>
#include <string>

extern "C" JNIEXPORT jint JNICALL
Java_com_electrom_process_MainProcess_startMainModule(
        JNIEnv* env,
        jobject obj,
        jobjectArray arguments) {
    jclass cls = env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(cls, "startRendererProcess", "()V");
    env->CallVoidMethod(obj, mid);
    return 0;
}