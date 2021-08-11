#include <jni.h>
#include <string>

extern "C" JNIEXPORT jint JNICALL
Java_com_electrom_node_process_MainProcess_startMainModule(
        JNIEnv* env,
        jobject clazz,
        jobjectArray arguments) {
    std::string hello = "Hello from C++";
    return 0;
}
