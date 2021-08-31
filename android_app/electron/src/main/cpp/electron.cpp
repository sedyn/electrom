#include "libnode/include/node/node.h"
#include "libnode/include/node/uv.h"
#include <jni.h>
#include <string>

#include "log.h"
#include "helper.h"

#include <android/log.h>

#include "android_context.h"
#include "electron_api_app.h"
#include "electron_api_browser_window.h"

using node::ArrayBufferAllocator;
using node::Environment;
using node::IsolateData;
using node::MultiIsolatePlatform;
using namespace v8;

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
    RegisterBrowserWindow(electron, eventEmitter);
}

jint RunNodeInstance(MultiIsolatePlatform *platform,
                     const std::vector<std::string> &args,
                     const std::vector<std::string> &exec_args,
                     const char *base_path) {
    int exit_code = 0;
    uv_loop_t* loop = uv_default_loop();
    int ret = uv_loop_init(loop);
    if (ret != 0) {
        log(ANDROID_LOG_ERROR, "Failed to initialize uv_loop");
        return 1;
    }

    std::shared_ptr<ArrayBufferAllocator> allocator = ArrayBufferAllocator::Create();

    Isolate *isolate = NewIsolate(allocator.get(), loop, platform);

    if (isolate == nullptr) {
        log(ANDROID_LOG_ERROR, "Failed to initialize V8 Isolate");
        return 1;
    }

    {
        Locker locker(isolate);
        Isolate::Scope isolate_scope(isolate);

        std::unique_ptr<IsolateData, decltype(&node::FreeIsolateData)> isolate_data(
                node::CreateIsolateData(isolate, loop, platform, allocator.get()),
                node::FreeIsolateData);

        HandleScope handle_scope(isolate);
        Local<Context> context = node::NewContext(isolate);

        if (context.IsEmpty()) {
            log(ANDROID_LOG_ERROR, "Failed to initialize V8 Context");
            return 1;
        }

        Context::Scope context_scope(context);
        std::unique_ptr<Environment, decltype(&node::FreeEnvironment)> env(
                node::CreateEnvironment(isolate_data.get(), context, args, exec_args),
                node::FreeEnvironment);

        Local<v8::Object> electronObj = v8::Object::New(isolate);

        SetElectronModule(electronObj);

        auto globalThis = context->Global();

        globalThis->Set(context, String::NewFromUtf8(isolate, "electron"), electronObj).Check();
        globalThis->Set(context, String::NewFromUtf8(isolate, "__dirname"), String::NewFromUtf8(isolate, base_path)).Check();
        NODE_SET_METHOD(globalThis, "initializeWithEventEmitter", &InitializeWithEventEmitter);

        MaybeLocal<Value> loadenv_ret = node::LoadEnvironment(env.get(), LOADER);

        if (loadenv_ret.IsEmpty())  // There has been a JS exception.
            return 1;

        {
            SealHandleScope seal(isolate);
            bool more;
            do {
                uv_run(loop, UV_RUN_DEFAULT);

                platform->DrainTasks(isolate);
                more = uv_loop_alive(loop);
                if (more) continue;

                node::EmitBeforeExit(env.get());
                more = uv_loop_alive(loop);
            } while (more);
        }

        exit_code = node::EmitExit(env.get());

        node::Stop(env.get());
    }

    bool platform_finished = false;
    platform->AddIsolateFinishedCallback(isolate, [](void *data) {
        *static_cast<bool *>(data) = true;
    }, &platform_finished);
    platform->UnregisterIsolate(isolate);
    isolate->Dispose();

    // Wait until the platform has cleaned up all relevant resources.
    while (!platform_finished)
        uv_run(loop, UV_RUN_ONCE);
    int err = uv_loop_close(loop);
    assert(err == 0);

    return exit_code;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_electrom_process_MainProcess_startMainModule(
        JNIEnv *env,
        jobject obj,
        jobjectArray arguments) {
    auto storagePath = (jstring) env->GetObjectArrayElement(arguments, 0);
    auto mainEntryFilePath = (jstring) env->GetObjectArrayElement(arguments, 1);

    const char *basePath = env->GetStringUTFChars(storagePath, 0);

    char fullPath[env->GetStringUTFLength(storagePath) + env->GetStringUTFLength(mainEntryFilePath) + 2];
    sprintf(fullPath, "%s/%s", basePath, env->GetStringUTFChars(mainEntryFilePath, 0));

    int argc = 2;
    char **argv = new char *[argc];

    argv[0] = (char *) "node";
    argv[1] = fullPath;

    if (start_redirecting_stdout_stderr() == -1) {
        log(ANDROID_LOG_ERROR, "Couldn't start redirecting stdout and stderr to logcat.");
    }

    InitAndroidContext(env, obj);

    argv = uv_setup_args(argc, argv);
    std::vector<std::string> args(argv, argv + argc);
    std::vector<std::string> exec_args;
    std::vector<std::string> errors;

    int exit_code = node::InitializeNodeWithArgs(&args, &exec_args, &errors);
    if (exit_code != 0) {
        log(ANDROID_LOG_ERROR, "Failed to initialize node");
        return exit_code;
    }

    std::unique_ptr<MultiIsolatePlatform> platform = MultiIsolatePlatform::Create(2);
    V8::InitializePlatform(platform.get());
    V8::Initialize();

    int ret = RunNodeInstance(platform.get(), args, exec_args, basePath);

    V8::Dispose();
    V8::ShutdownPlatform();

    return ret;
}
