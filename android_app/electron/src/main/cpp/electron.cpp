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

ElectronHandler *electron = nullptr;

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

std::string ConvertJStringToString(JNIEnv *env, jobject obj) {
    return std::string(env->GetStringUTFChars((jstring) obj, nullptr));
}

ElectronModulePaths *ParseArguments(JNIEnv *env,
                                    jobjectArray arguments) {
    auto *electron_module_paths = new ElectronModulePaths;

    /**
     * [0] = electron app folder path
     * [1] = electron internal script folder path
     * [2] = electron startup script name
     */
    electron_module_paths->browserInitScript =
            ConvertJStringToString(env, env->GetObjectArrayElement(arguments, 0));
    electron_module_paths->assetsPackage =
            ConvertJStringToString(env, env->GetObjectArrayElement(arguments, 1));
    electron_module_paths->mainStartupScript =
            ConvertJStringToString(env, env->GetObjectArrayElement(arguments, 2));

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

    if (electron == nullptr) {
        AndroidContext::Initialize(env, thiz);
        electron = new ElectronHandler(electron_module_paths);
        electron->Initialize();
        electron->RunMessageLoop();
    }
    return 0;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_electrom_process_MainProcess_uvRunOnce(JNIEnv *env, jobject thiz) {
    electron->UvRunOnce();
}