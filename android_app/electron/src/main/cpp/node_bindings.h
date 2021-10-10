#ifndef ANDROID_APP_NODE_BINDINGS_H
#define ANDROID_APP_NODE_BINDINGS_H

#include "libnode/include/node/uv.h"
#include "libnode/include/node/node.h"

/**
 * https://github.com/electron/electron/blob/main/shell/common/node_bindings.h
 */
class NodeBinding {
public:
    NodeBinding();

    uv_loop_t *uv_loop() const { return uv_loop_; }

    // Gets/sets the environment to wrap uv loop.
    void set_uv_env(node::Environment *env) { uv_env_ = env; }

    node::Environment *uv_env() const { return uv_env_; }

    void Initialize(const char *main_module_path);

    void LoadEnvironment(node::Environment *env);

    void PrepareMessageLoop();

    void RunMessageLoop();

    void UvRunOnce();

    node::Environment *CreateEnvironment(
            v8::Handle<v8::Context> context,
            node::MultiIsolatePlatform *platform,
            const char *main_module_path
    );

private:
    static void EmbedThreadRunner(void *arg);

    void PollEvents();

    bool embed_closed_ = false;

    uv_loop_t *uv_loop_;
    uv_async_t *dummy_uv_handle_;
    uv_thread_t embed_thread_;
    uv_sem_t embed_sem_;

    // Environment that to wrap the uv loop.
    node::Environment *uv_env_ = nullptr;

    // Isolate data used in creating the environment
    node::IsolateData *isolate_data_ = nullptr;

    int epoll_;
};

#endif //ANDROID_APP_NODE_BINDINGS_H
