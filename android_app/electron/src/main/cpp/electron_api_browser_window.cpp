#include "electron_api_browser_window.h"
#include "android_context.h"
#include "helper.h"
#include "libnode/include/node/uv.h"

#include <thread>

using namespace v8;

void BrowserWindowConstructor(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    Local<Object> properties = args[0]->ToObject(isolate);

    Local<Object> browserWindow = args.Holder();
    const char *processId = android()->StartRendererProcess(stringify(properties).c_str());

    browserWindow->Set(String::NewFromUtf8(isolate, "id"), String::NewFromUtf8(isolate, processId));
}

struct Item {
    std::string url;
    Persistent<Promise::Resolver> *resolver;
    Persistent<Object> *holder;
};

void callbackLoadURL(const AndroidContext *ctx, void *data) {
    auto msg = [](uv_async_t *handle) {
        auto data = (Item*) handle->data;

        Isolate *isolate = Isolate::GetCurrent();
        HandleScope handle_scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        Local<Promise::Resolver> resolver = data->resolver->Get(isolate);
        Local<Object> holder = data->holder->Get(isolate);

        resolver->Resolve(context, String::NewFromUtf8(isolate, "test")).Check();

        Local<Value> argv[] = {
                (Local<Value>) String::NewFromUtf8(isolate, "ready-to-show"),
        };

        holder->Get(String::NewFromUtf8(isolate, "emit")).As<Function>()->Call(context, holder, 1, argv).ToLocalChecked();
        delete data->resolver;
        delete data->holder;
        free(data);
        uv_close((uv_handle_t*) handle, [](uv_handle_t* handle) {
            delete handle;
        });
    };

    auto async = new uv_async_t();
    uv_loop_t *loop = uv_default_loop();
    uv_async_init(loop, async, msg);

    async->data = data;

    uv_async_send(async);

    ctx->CommandToRendererProcess("loadURL", ((Item*)data)->url.c_str());
}

void loadURL(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    Local<String> url = args[0]->ToString(isolate);

    auto resolver = Promise::Resolver::New(context).ToLocalChecked();

    auto data = new Item{
            std::string(*String::Utf8Value(isolate, url)),
            new Persistent<Promise::Resolver>(isolate, resolver),
            new Persistent<Object>(isolate, args.Holder())
    };

    RequestThread(callbackLoadURL, data);

    args.GetReturnValue().Set(resolver->GetPromise());
}

void show(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    android()->CommandToRendererProcess(__func__, nullptr);
}

void RegisterBrowserWindow(Local<Object> electron, Local<Object> eventEmitter) {
    Isolate *isolate = electron->GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    Local<String> browserWindowName = String::NewFromUtf8(isolate, "BrowserWindow");

    Local<FunctionTemplate> BrowserWindowTemplate = FunctionTemplate::New(isolate, &BrowserWindowConstructor);
    BrowserWindowTemplate->SetClassName(browserWindowName);
    NODE_SET_PROTOTYPE_METHOD(BrowserWindowTemplate, "loadURL", &loadURL);
    NODE_SET_PROTOTYPE_METHOD(BrowserWindowTemplate, "show", &show);

    Local<Function> BrowserWindow = BrowserWindowTemplate->GetFunction(context).ToLocalChecked();
    BrowserWindow->SetName(browserWindowName);

    // event_emitter_mixin.cc 참고
    Local<Value> BrowserWindowPrototype = BrowserWindow->Get(context, String::NewFromUtf8(isolate, "prototype")).ToLocalChecked();
    BrowserWindowPrototype.As<Object>()->SetPrototype(context, eventEmitter).Check();

    electron->Set(context, browserWindowName, BrowserWindow).Check();
}
