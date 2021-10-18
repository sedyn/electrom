#include "libnode/include/node/node.h"
#include "libnode/include/node/uv.h"

#include <jni.h>
#include <string>

#include "log.h"
#include "helper.h"

#include <android/log.h>

#include "android_context.h"

#include "electron_main_parts.h"

#include "electron_api_app.h"
#include "electron_api_browser_window.h"
#include "electron_api_ipc_main.h"

using node::Environment;
using node::IsolateData;
using node::MultiIsolatePlatform;
using namespace v8;

class ElectronHandler {
public:
    explicit ElectronHandler(ElectronModulePaths *electron_module_paths);

    void Initialize();

    void RunMessageLoop();

    void UvRunOnce();

private:
    std::unique_ptr<ElectronMainParts> main_parts_;
};

ElectronHandler::ElectronHandler(ElectronModulePaths *electron_module_paths) {
    main_parts_ = std::make_unique<ElectronMainParts>(electron_module_paths);
}

void ElectronHandler::Initialize() {
    main_parts_->Initialize();
}

void ElectronHandler::RunMessageLoop() {
    main_parts_->RunMessageLoop();
}

void ElectronHandler::UvRunOnce() {
    main_parts_->UvRunOnce();
}

ElectronHandler *handler = nullptr;

std::string ConvertJStringToString(JNIEnv *env, jobjectArray arguments, int index) {
    return std::string(
            env->GetStringUTFChars((jstring) env->GetObjectArrayElement(arguments, index),
                                   nullptr));
}

ElectronModulePaths *ParseArguments(JNIEnv *env,
                                    jobjectArray arguments) {
    auto *electron_module_paths = new ElectronModulePaths;

    /**
     * [0] = electron app folder path
     * [1] = electron internal script folder path
     * [2] = electron startup script name
     */
    electron_module_paths->browserInitScript = ConvertJStringToString(env, arguments, 0);
    electron_module_paths->assetsPackage = ConvertJStringToString(env, arguments, 1);
    electron_module_paths->mainStartupScript = ConvertJStringToString(env, arguments, 2);

    return electron_module_paths;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_electrom_process_MainProcess_startMainModule(
        JNIEnv *env,
        jobject thiz,
        jobjectArray arguments) {
    ElectronModulePaths *electron_module_paths = ParseArguments(env, arguments);

    if (start_redirecting_stdout_stderr() == -1) {
        log(ANDROID_LOG_ERROR, "Couldn't start redirecting stdout and stderr to logcat.");
    }

    if (handler == nullptr) {
        AndroidContext::Initialize(env, thiz);
        handler = new ElectronHandler(electron_module_paths);
        handler->Initialize();
        handler->RunMessageLoop();
        App::Get()->EmitReady();
    }
    return 0;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_electrom_process_MainProcess_uvRunOnce(JNIEnv *env, jobject thiz) {
    handler->UvRunOnce();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_electrom_process_MainProcess_emitWebContentsEvent(
        JNIEnv *env,
        jobject thiz,
        jint web_contents_id,
        jstring event) {
    Isolate *isolate = JavascriptEnvironment::GetIsolate();
    BrowserWindow *browserWindow = BrowserWindow::FromWeakMapID(isolate, web_contents_id);
    browserWindow->Emit(env->GetStringUTFChars(event, nullptr));
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_electrom_process_MainProcess_emitIpcMainSync(
        JNIEnv *env,
        jobject thiz,
        jstring event,
        jstring data) {
    std::string result = IpcMain::Get()->HandleSyncEvent(
            env->GetStringUTFChars(event, nullptr),
            env->GetStringUTFChars(data, nullptr)
    );

    return env->NewStringUTF(result.c_str());
}
