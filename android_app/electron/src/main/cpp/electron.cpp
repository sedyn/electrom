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

using node::Environment;
using node::IsolateData;
using node::MultiIsolatePlatform;
using namespace v8;

class ElectronHandler {
public:
    ElectronHandler();

    void Initialize(const char *main_module_path);

    void RunMessageLoop();

    void UvRunOnce();

private:
    std::unique_ptr<ElectronMainParts> main_parts_;
};

ElectronHandler::ElectronHandler() {
    main_parts_ = std::make_unique<ElectronMainParts>();
}

void ElectronHandler::Initialize(const char *main_module_path) {
    main_parts_->Initialize(main_module_path);
}

void ElectronHandler::RunMessageLoop() {
    main_parts_->RunMessageLoop();
}

void ElectronHandler::UvRunOnce() {
    main_parts_->UvRunOnce();
}

ElectronHandler *electron() {
    static auto *electron = new ElectronHandler;
    return electron;
}

const char *LOADER =
        "const electron = globalThis.electron;"
        "delete globalThis.electron;" // Environment를 현재는 접근할 수 없어서 이 방식을 선택한다.
        "const publicRequire = require('module').createRequire(process.cwd() + '/');"
        "globalThis.initializeWithEventEmitter(electron, publicRequire('events').EventEmitter.prototype);"
        "delete globalThis.initializeWithEventEmitter;"
        "const electronRequire = function(modulePath) {"
        "  if (modulePath === 'electron') { " // intercept electron module require
        "    return electron;"
        "  } else {"
        "    return publicRequire(modulePath);"
        "  }"
        "};"
        "console.log(electron);"
        "globalThis.require = electronRequire;"
        "const mainCode = require('fs').readFileSync(process.argv[1]);"
        "require('vm').runInThisContext(mainCode);"
        "electron.app.emit('ready');"
        "console.log('after ready');";

void SetElectronModule(Local<Object> electron) {}

void InitializeWithEventEmitter(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();

    Local<Object> electron = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
    Local<Object> eventEmitter = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();

    app()->init(electron, eventEmitter);
    // RegisterBrowserWindow(electron, eventEmitter);
}


extern "C" JNIEXPORT jint JNICALL
Java_com_electrom_process_MainProcess_startMainModule(
        JNIEnv *env,
        jobject thiz,
        jobjectArray arguments) {
    auto storagePath = (jstring) env->GetObjectArrayElement(arguments, 0);
    auto mainEntryFilePath = (jstring) env->GetObjectArrayElement(arguments, 1);

    const char *basePath = env->GetStringUTFChars(storagePath, 0);

    char fullPath[
            env->GetStringUTFLength(storagePath) + env->GetStringUTFLength(mainEntryFilePath) + 2];
    sprintf(fullPath, "%s/%s", basePath, env->GetStringUTFChars(mainEntryFilePath, 0));

    if (start_redirecting_stdout_stderr() == -1) {
        log(ANDROID_LOG_ERROR, "Couldn't start redirecting stdout and stderr to logcat.");
    }

    InitAndroidContext(env, thiz);
    electron()->Initialize(fullPath);
    electron()->RunMessageLoop();

    return 0;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_electrom_process_MainProcess_uvRunOnce(JNIEnv *env, jobject thiz) {
    electron()->UvRunOnce();
}