#include "node_bindings.h"
#include "log.h"
#include "android_context.h"
#include "libnode/include/node/env-inl.h"

#include <sys/epoll.h>
#include <vector>

using namespace v8;

#define ELECTRON_BUILTIN_MODULES(V) \
    V(electron_browser_window) \
    V(electron_browser_event_emitter) \
    V(electron_browser_app)

#define V(modname) void _register_##modname();

ELECTRON_BUILTIN_MODULES(V)

#undef V

NodeBinding::NodeBinding(ElectronModulePaths *electron_module_paths) :
        electron_module_paths_(electron_module_paths) {
    uv_loop_ = uv_default_loop();
    dummy_uv_handle_ = new uv_async_t;

    // from node_binding_linux.cc
    epoll_ = epoll_create(1);
    int backend_fd = uv_backend_fd(uv_loop_);
    struct epoll_event ev = {0};
    ev.events = EPOLLIN;
    ev.data.fd = backend_fd;
    epoll_ctl(epoll_, EPOLL_CTL_ADD, backend_fd, &ev);
}

void NodeBinding::Initialize() {
#define V(modname) _register_##modname();
    ELECTRON_BUILTIN_MODULES(V)
#undef V

    std::vector<std::string> argv = {"electron"};
    std::vector<std::string> exec_argv;
    std::vector<std::string> errors;

    int exit_code = node::InitializeNodeWithArgs(&argv, &exec_argv, &errors);
    if (exit_code != 0) {
        return;
    }
}

void NodeBinding::LoadEnvironment(node::Environment *env) {
    node::LoadEnvironment(env, node::StartExecutionCallback{});
}

node::Environment *NodeBinding::CreateEnvironment(
        v8::Handle<v8::Context> context,
        node::MultiIsolatePlatform *platform
) {
    isolate_data_ = node::CreateIsolateData(context->GetIsolate(), uv_loop_, platform);

    // args.insert(args.begin() + 1, "electron/js2c/browser_init");
    std::vector<std::string> args = {
            "electron",
            electron_module_paths_->browserInitScript + "/" + "browser-init.js",
            electron_module_paths_->assetsPackage + "/" + electron_module_paths_->mainStartupScript
    };
    std::vector<std::string> exec_args;

    node::Environment *env = node::CreateEnvironment(
            isolate_data_,
            context,
            args,
            exec_args
    );

    if (env == nullptr) {
        return nullptr;
    }

    node::IsolateSettings is;

    is.should_abort_on_uncaught_exception_callback = [](Isolate *) {
        return false;
    };

    is.policy = v8::MicrotasksPolicy::kExplicit;

    node::SetIsolateUpForNode(context->GetIsolate(), is);

    return env;
}

// from node_binding_linux.cc
void NodeBinding::PollEvents() {
    int timeout = uv_backend_timeout(uv_loop_);

    int r;
    do {
        struct epoll_event ev = {0};
        r = epoll_wait(epoll_, &ev, 1, timeout);
    } while (r == -1 && errno == EINTR);
}

void NodeBinding::EmbedThreadRunner(void *arg) {
    auto env = AttachCurrentThread();
    auto *self = static_cast<NodeBinding *>(arg);

    while (true) {
        uv_sem_wait(&self->embed_sem_);
        if (self->embed_closed_)
            break;

        self->PollEvents();
        if (self->embed_closed_)
            break;

        LOG_INFO("WakeUpMainThread")
        // Register task to Android UI Thread Looper queue.
        // Instead of ThreadTaskRunnerHandle in electron.
        AddTaskForMainLooper(env);
    }

    DetachCurrentThread();
}

void NodeBinding::PrepareMessageLoop() {
    // Add dummy handle for libuv, otherwise libuv would quit when there is
    // nothing to do.
    uv_async_init(
            uv_loop_,
            dummy_uv_handle_,
            nullptr
    );

    uv_sem_init(&embed_sem_, 0);
    uv_thread_create(&embed_thread_, EmbedThreadRunner, this);
}

void NodeBinding::RunMessageLoop() {
    uv_loop_->data = this;
    UvRunOnce();
}

// Simple copy of gin_helper::Locker
class ElectronLocker {
public:
    explicit ElectronLocker(Isolate *isolate);

    ~ElectronLocker();

    static inline bool IsBrowserProcess() { return Locker::IsActive(); }

private:
    std::unique_ptr<Locker> locker_;
};

ElectronLocker::ElectronLocker(Isolate *isolate) {
    if (IsBrowserProcess())
        locker_ = std::make_unique<Locker>(isolate);
}

ElectronLocker::~ElectronLocker() = default;

void NodeBinding::UvRunOnce() {
    node::Environment *env = uv_env();
    if (!env) {
        return;
    }

    ElectronLocker locker(env->isolate());

    HandleScope handle_scope(env->isolate());

    Context::Scope context_scope(env->context());

    auto old_policy = env->isolate()->GetMicrotasksPolicy();
    env->isolate()->SetMicrotasksPolicy(MicrotasksPolicy::kExplicit);

    int r = uv_run(uv_loop_, UV_RUN_NOWAIT);

    env->isolate()->SetMicrotasksPolicy(old_policy);

    if (r == 0)
        // TODO impl base::RunLoop().QuitWhenIdle
        LOG_INFO("QuitWhenIdle")

    uv_sem_post(&embed_sem_);
}
